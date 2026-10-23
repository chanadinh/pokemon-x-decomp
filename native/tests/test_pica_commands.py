"""Synthetic PICA bounds, byte masks, provenance and float32 upload checks."""
import importlib.util
from pathlib import Path
import struct
import unittest


spec = importlib.util.spec_from_file_location(
    'pica_commands', Path(__file__).resolve().parents[1] / 'tools/pica_commands.py')
pica = importlib.util.module_from_spec(spec)
spec.loader.exec_module(pica)


def packet(register, values, mask=0xF, consecutive=False, padding=0):
    header = (register | (mask << 16) | ((len(values) - 1) << 20)
              | (int(consecutive) << 31))
    words = [values[0], header, *values[1:]]
    if len(words) & 1:
        words.append(padding)
    return struct.pack('<' + 'I' * len(words), *words)


def float_words(*values):
    return [struct.unpack('<I', struct.pack('<f', value))[0] for value in values]


def decode(data):
    return pica.decode_commands(data, 0, len(data) // 4)


class PICACommandTests(unittest.TestCase):
    def test_asymmetric_byte_masks_and_provenance(self):
        first = packet(0x203, [0x12345678])
        result = decode(first + packet(0x203, [0xA1B2C3D4], mask=0xA))
        self.assertEqual(result.registers[0x203], 0xA134C378)
        self.assertEqual(result.register_known_masks[0x203], 0xF)
        self.assertNotIn(0x203, result.register_sources)
        result = decode(first + packet(0x203, [0xA1B2C3D4], mask=0x5))
        self.assertEqual(result.registers[0x203], 0x12B256D4)
        result = decode(first + packet(0x203, [0], mask=0))
        self.assertEqual(result.registers[0x203], 0x12345678)
        self.assertEqual(result.register_sources[0x203], 0)

    def test_unknown_bytes_do_not_become_known_zeroes(self):
        first = packet(0x203, [0xAABBCCDD], mask=0x5)
        result = decode(first)
        self.assertNotIn(0x203, result.registers)
        self.assertNotIn(0x203, result.register_sources)
        self.assertEqual(result.register_known_masks[0x203], 0x5)
        self.assertEqual(result.register_values[0x203], 0x00BB00DD)
        result = decode(first + packet(0x203, [0x11223344], mask=0xA))
        self.assertEqual(result.registers[0x203], 0x11BB33DD)
        self.assertNotIn(0x203, result.register_sources)
        result = decode(first + packet(0x203, [0x01234567]))
        self.assertEqual(result.register_sources[0x203], 8)

    def test_repeated_consecutive_writes_and_absolute_sources(self):
        data = (b'HEAD' + packet(0x200, [10, 20, 30, 40], consecutive=True)
                + packet(0x227, [50, 60, 70]))
        result = pica.decode_commands(data, 4, (len(data) - 4) // 4)
        self.assertEqual(result.registers,
                         {0x200: 10, 0x201: 20, 0x202: 30, 0x203: 40, 0x227: 70})
        self.assertEqual(result.register_sources[0x203], 20)
        self.assertEqual(result.register_sources[0x227], 40)
        self.assertEqual([write.source_offset for write in result.writes],
                         [4, 12, 16, 20, 28, 36, 40])
        self.assertEqual([write.register for write in result.writes][-3:],
                         [0x227, 0x227, 0x227])

    def test_f32_order_auto_increment_and_consecutive_index(self):
        data = (packet(0x2C0, [0x80000006, *float_words(4, 3, 2, 1)], consecutive=True)
                + packet(0x2C0, [0x80000007])
                + packet(0x2C1, float_words(8, 7, 6, 5, 12, 11, 10, 9)))
        self.assertEqual(decode(data).uniforms,
                         {6: (1, 2, 3, 4), 7: (5, 6, 7, 8), 8: (9, 10, 11, 12)})

    def test_uniform_data_can_span_packets(self):
        data = (packet(0x2C0, [0x80000006])
                + packet(0x2C1, float_words(4, 3))
                + packet(0x2C8, float_words(2, 1)))
        self.assertEqual(decode(data).uniforms[6], (1, 2, 3, 4))

    def test_incomplete_or_overwritten_uniform_is_not_reported(self):
        complete = (packet(0x2C0, [0x80000006])
                    + packet(0x2C1, float_words(4, 3, 2, 1)))
        incomplete = packet(0x2C0, [0x80000006]) + packet(0x2C1, float_words(10))
        self.assertEqual(decode(incomplete).uniforms, {})
        self.assertEqual(decode(complete + incomplete).uniforms, {})
        self.assertEqual(decode(packet(0x2C0, [0x80000005])
                                + packet(0x2C1, float_words(4, 3, 2, 1))).uniforms, {})

    def test_unsupported_or_uninitialized_uniforms(self):
        index = packet(0x2C0, [0x80000006])
        invalid = [packet(0x2C0, [6]),
                   packet(0x2C0, [0x80000006], mask=8),
                   packet(0x2C0, [0x80000060]),
                   packet(0x2C1, [0]),
                   index + packet(0x2C1, [0], mask=1),
                   index + packet(0x2C1, float_words(float('nan'))),
                   index + packet(0x2C1, float_words(float('inf'))),
                   index + packet(0x2C1, float_words(-float('inf'))),
                   packet(0x2C0, [0x8000005F]) + packet(0x2C1, [0] * 5)]
        for data in invalid:
            with self.subTest(data=data.hex()), self.assertRaises(pica.PICACommandError):
                decode(data)
        self.assertEqual(decode(packet(0x2C1, [0], mask=0)).uniforms, {})

    def test_padding_is_required_but_its_value_is_uninterpreted(self):
        data = packet(0x123, [4, 5], padding=0xFFFFFFFF)
        self.assertEqual(decode(data).registers, {0x123: 5})
        with self.assertRaisesRegex(pica.PICACommandError, 'padding'):
            pica.decode_commands(data, 0, 3)
        # Bytes after the declared extent cannot supply missing parameters.
        with self.assertRaisesRegex(pica.PICACommandError, 'parameters'):
            pica.decode_commands(data, 0, 2)
        with self.assertRaisesRegex(pica.PICACommandError, 'header'):
            pica.decode_commands(data, 0, 1)

    def test_all_truncated_byte_extents_and_exact_extent(self):
        data = packet(0x200, [1, 2, 3, 4], consecutive=True)
        for length in range(len(data)):
            with self.subTest(length=length), self.assertRaises(pica.PICACommandError):
                pica.decode_commands(data[:length], 0, len(data) // 4)
        result = pica.decode_commands(data + b'outside', 0, len(data) // 4)
        self.assertEqual(result.registers[0x203], 4)

    def test_register_ids_and_consecutive_overflow(self):
        self.assertEqual(decode(packet(0x3FF, [1])).registers, {0x3FF: 1})
        for data in [packet(0x400, [1]), packet(0xFFFF, [1]),
                     packet(0x3FF, [1, 2], consecutive=True)]:
            with self.subTest(data=data.hex()), self.assertRaises(pica.PICACommandError):
                decode(data)

    def test_budgets_alignment_types_and_empty_stream(self):
        data = packet(0x100, [1])
        cases = [(-4, 2, 2), (1, 1, 2), (12, 0, 2), (0, -1, 2),
                 (0, 3, 2), (0, 2, -1), (0, 2, pica.MAX_WORDS + 1),
                 (0, pica.MAX_WORDS + 1, pica.MAX_WORDS),
                 (0.0, 2, 2), (0, True, 2), (0, 2, 2.0)]
        for offset, count, budget in cases:
            with self.subTest(case=(offset, count, budget)), self.assertRaises(pica.PICACommandError):
                pica.decode_commands(data, offset, count, max_words=budget)
        self.assertEqual(pica.decode_commands(data, 0, 2, max_words=2).registers, {0x100: 1})
        self.assertEqual(pica.decode_commands(data, 8, 0).writes, ())

    def test_input_is_unchanged(self):
        data = bytearray(packet(0x203, [0x12345678]))
        original = bytes(data)
        decode(data)
        self.assertEqual(bytes(data), original)


if __name__ == '__main__':
    unittest.main()
