"""Synthetic code-set tests; no game binaries or external packages required."""

import hashlib
import importlib.util
import json
from pathlib import Path
import struct
import tempfile
import unittest


SCRIPT = Path(__file__).resolve().parents[1] / "scripts" / "inspect_code.py"
SPEC = importlib.util.spec_from_file_location("inspect_code", SCRIPT)
inspect_code = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(inspect_code)


class IntakeTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name)
        self.code = self.root / "code.bin"
        self.exheader = self.root / "exheader.bin"
        self.header = bytearray(0x400)
        self.header[:8] = b"SYNTHET\0"
        struct.pack_into("<H", self.header, 0x0E, 3)
        struct.pack_into("<I", self.header, 0x1C, 0x4000)
        struct.pack_into("<Q", self.header, 0x200, 0x0004000000000000)
        # Deliberately separated virtual addresses: file offsets must use page
        # counts, not subtract the .text address from other virtual addresses.
        struct.pack_into("<III", self.header, 0x10, 0x100000, 1, 8)
        struct.pack_into("<III", self.header, 0x20, 0x200000, 1, 16)
        struct.pack_into("<III", self.header, 0x30, 0x300000, 1, 16)
        struct.pack_into("<I", self.header, 0x3C, 32)
        self.code.write_bytes(bytes(0x3000))
        self.exheader.write_bytes(self.header)

    def inspect(self, decompressed=False):
        self.exheader.write_bytes(self.header)
        return inspect_code.inspect_pair(self.code, self.exheader, decompressed)

    def test_padded_map_uses_actual_sizes_and_page_strides(self):
        result = self.inspect()
        self.assertEqual(result["layout"], "page-padded")
        self.assertEqual(result["entry_point"], 0x100000)
        segments = result["segments"]
        self.assertEqual([s["offset"] for s in segments], [0, 0x1000, 0x2000, None])
        self.assertEqual([s["size"] for s in segments], [8, 16, 16, 32])
        self.assertEqual(segments[-1]["address"], 0x300010)
        self.assertTrue(segments[0]["execute"])
        self.assertFalse(segments[0]["write"])
        self.assertTrue(segments[-1]["write"])
        self.assertFalse(segments[-1]["execute"])

    def test_packed_map(self):
        self.code.write_bytes(bytes(40))
        result = self.inspect()
        self.assertEqual(result["layout"], "packed")
        self.assertEqual([s["offset"] for s in result["segments"]], [0, 8, 24, None])
        self.assertEqual(result["segments"][-1]["address"], 0x300010)

    def test_hashes_and_metadata(self):
        result = self.inspect()
        self.assertEqual(result["code"]["sha256"], hashlib.sha256(self.code.read_bytes()).hexdigest())
        self.assertEqual(result["exheader"]["sha256"], hashlib.sha256(self.header).hexdigest())
        self.assertEqual(result["metadata"]["application_name"], "SYNTHET")
        self.assertEqual(result["metadata"]["program_id"], "0004000000000000")
        self.assertEqual(result["metadata"]["remaster_version"], 3)

    def test_full_exheader_is_accepted(self):
        self.header.extend(bytes(0x400))
        self.assertEqual(self.inspect()["exheader"]["size"], 0x800)

    def test_other_exheader_lengths_are_rejected(self):
        for length in (0, 0x200, 0x3FF, 0x401, 0x7FF, 0x801):
            with self.subTest(length=length):
                self.exheader.write_bytes(bytes(length))
                with self.assertRaisesRegex(inspect_code.IntakeError, "0x400 or 0x800"):
                    inspect_code.inspect_pair(self.code, self.exheader)

    def test_compression_flag_requires_explicit_assertion(self):
        self.header[0x0D] = 1
        with self.assertRaisesRegex(inspect_code.IntakeError, "--decompressed"):
            self.inspect()
        result = self.inspect(decompressed=True)
        self.assertTrue(result["metadata"]["exheader_compressed_flag"])
        self.assertTrue(result["metadata"]["decompressed_asserted"])
        self.assertIn("compression flag", result["warnings"][-1])

    def test_decompressed_assertion_does_not_bypass_length_check(self):
        self.header[0x0D] = 1
        self.code.write_bytes(bytes(100))
        with self.assertRaisesRegex(inspect_code.IntakeError, "does not match"):
            self.inspect(decompressed=True)

    def test_truncated_extra_and_partially_padded_code_are_rejected(self):
        for size in (0, 39, 41, 0x2000 + 16, 0x2FFF, 0x3001):
            with self.subTest(size=size):
                self.code.write_bytes(bytes(size))
                with self.assertRaisesRegex(inspect_code.IntakeError, "does not match"):
                    self.inspect()

    def test_size_cannot_exceed_page_count(self):
        struct.pack_into("<I", self.header, 0x18, 0x1001)
        with self.assertRaisesRegex(inspect_code.IntakeError, "exceeds.*page"):
            self.inspect()

    def test_nonzero_size_with_zero_pages_is_rejected(self):
        struct.pack_into("<I", self.header, 0x14, 0)
        with self.assertRaisesRegex(inspect_code.IntakeError, "exceeds.*page"):
            self.inspect()

    def test_unaligned_section_address_is_rejected(self):
        struct.pack_into("<I", self.header, 0x20, 0x200004)
        with self.assertRaisesRegex(inspect_code.IntakeError, "page-aligned"):
            self.inspect()

    def test_overlapping_page_allocations_are_rejected(self):
        struct.pack_into("<I", self.header, 0x20, 0x100000)
        with self.assertRaisesRegex(inspect_code.IntakeError, "allocations overlap"):
            self.inspect()

    def test_32_bit_section_overflow_is_rejected(self):
        struct.pack_into("<III", self.header, 0x30, 0xFFFFF000, 2, 16)
        with self.assertRaisesRegex(inspect_code.IntakeError, "32-bit"):
            self.inspect()

    def test_bss_starts_after_actual_data_not_its_padding(self):
        struct.pack_into("<I", self.header, 0x3C, 0x2000)
        self.assertEqual(self.inspect()["segments"][-1]["address"], 0x300010)

    def test_bss_32_bit_overflow_is_rejected(self):
        struct.pack_into("<I", self.header, 0x30, 0xFFFFF000)
        struct.pack_into("<I", self.header, 0x3C, 0x2000)
        with self.assertRaisesRegex(inspect_code.IntakeError, "BSS exceeds"):
            self.inspect()

    def test_bss_cannot_overlap_other_sections(self):
        struct.pack_into("<I", self.header, 0x20, 0x301000)
        struct.pack_into("<I", self.header, 0x3C, 0x2000)
        with self.assertRaisesRegex(inspect_code.IntakeError, "BSS overlaps"):
            self.inspect()

    def test_zero_bss_is_omitted(self):
        struct.pack_into("<I", self.header, 0x3C, 0)
        self.assertEqual(len(self.inspect()["segments"]), 3)

    def test_bss_only_data_address_must_be_page_aligned(self):
        struct.pack_into("<III", self.header, 0x30, 0x300001, 0, 0)
        self.code.write_bytes(bytes(0x2000))
        with self.assertRaisesRegex(inspect_code.IntakeError, "page-aligned"):
            self.inspect()

    def test_bss_only_data_is_supported(self):
        struct.pack_into("<III", self.header, 0x30, 0x300000, 0, 0)
        self.code.write_bytes(bytes(0x2000))
        result = self.inspect()
        self.assertEqual([s["name"] for s in result["segments"]], [".text", ".rodata", ".bss"])
        self.assertEqual(result["segments"][-1]["address"], 0x300000)

    def test_cli_writes_json(self):
        output = self.root / "out" / "manifest.json"
        self.assertEqual(inspect_code.main([
            "--code", str(self.code), "--exheader", str(self.exheader), "--output", str(output)
        ]), 0)
        result = json.loads(output.read_text())
        self.assertEqual(result["segments"][-1]["offset"], None)

    def test_cli_cannot_overwrite_inputs(self):
        original = self.code.read_bytes()
        self.assertEqual(inspect_code.main([
            "--code", str(self.code), "--exheader", str(self.exheader), "--output", str(self.code)
        ]), 2)
        self.assertEqual(self.code.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
