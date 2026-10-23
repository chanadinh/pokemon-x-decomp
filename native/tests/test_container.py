"""Independent synthetic CGFX indexes; no game input or SDL dependency."""
import json
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import unittest

TOOLS = Path(__file__).resolve().parents[1] / "tools"
sys.path.insert(0, str(TOOLS))
from inspect_container import FormatError, inspect_cgfx, load_selected


def put32(data, offset, value):
    struct.pack_into("<I", data, offset, value & 0xffffffff)


def fixture():
    # Fixed layout: DATA [20,1024), IMAG [1024,1152). Two DICTs,
    # one model and one inline ETC1 texture; all values chosen independently.
    data = bytearray(1152)
    data[:20] = b"CGFX" + struct.pack("<HHIII", 0xfeff, 20, 0x05000000, 1152, 2)
    data[20:28] = b"DATA" + struct.pack("<I", 1004)
    data[1024:1032] = b"IMAG" + struct.pack("<I", 128)
    for slot, dictionary, obj, name in [(0, 156, 256, 248), (1, 200, 512, 784)]:
        put32(data, 28 + slot * 8, 1)
        put32(data, 32 + slot * 8, dictionary - (32 + slot * 8))
        data[dictionary:dictionary + 12] = b"DICT" + struct.pack("<II", 44, 1)
        record = dictionary + 28
        put32(data, record + 8, name - (record + 8))
        put32(data, record + 12, obj - (record + 12))
    data[248:254] = b"model\0"
    data[784:790] = b"image\0"
    put32(data, 256, 0x40000092)
    data[260:264] = b"CMDL"
    put32(data, 268, 248 - 268)  # A valid backwards relative pointer.
    for offset, value in [(0xb4, 2), (0xbc, 3), (0xc4, 4), (0xcc, 5)]:
        put32(data, 256 + offset, value)
    put32(data, 512, 0x20000011)
    data[516:520] = b"TXOB"
    put32(data, 524, 784 - 524)
    for offset, value in [(0x18, 5), (0x1c, 6), (0x28, 1), (0x34, 12),
                          (0x3c, 8), (0x40, 8), (0x44, 32),
                          (0x48, 1032 - (512 + 0x48)), (0x50, 4)]:
        put32(data, 512 + offset, value)
    return data


def garc(member):
    size = len(member)
    header = b"CRAG" + struct.pack("<IHHHHIII", 28, 0xfeff, 0x0400, 4, 0, 84, 84 + size, size)
    fato = b"OTAF" + struct.pack("<IHHI", 16, 1, 0xffff, 0)
    fatb = b"BTAF" + struct.pack("<IIIIII", 28, 1, 1, 0, size, size)
    return header + fato + fatb + b"BMIF" + struct.pack("<II", 12, size) + member


class ContainerTests(unittest.TestCase):
    def test_fixed_index_offsets_names_model_and_texture_metadata(self):
        result = inspect_cgfx(bytes(fixture()))
        self.assertEqual(result["blocks"], [
            {"magic": "DATA", "offset": 20, "size": 1004},
            {"magic": "IMAG", "offset": 1024, "size": 128}])
        model, texture = (category["records"][0] for category in result["categories"])
        self.assertEqual((model["record_offset"], model["object_offset"], model["name"]),
                         (184, 256, "model"))
        self.assertEqual(model["model"], {"name": "model", "name_hex": "6d6f64656c",
                                          "name_offset": 248, "has_skeleton_flag": True,
                                          "object_count": 2, "material_count": 3,
                                          "shape_count": 4, "object_node_count": 5})
        self.assertEqual(texture["texture"], {
            "status": "indexed", "name": "image", "name_hex": "696d616765", "name_offset": 784,
            "height": 5, "width": 6, "mipmap_levels": 1, "format": 12, "format_name": "ETC1",
            "stored_height": 8, "stored_width": 8, "data_offset": 1032, "data_size": 32,
            "bits_per_pixel": 4})

    def test_truncated_prefixes_rejected(self):
        data = fixture()
        for length in [0, 4, 19, 20, 27, 155, 199, 255, 511, 1024, 1151]:
            with self.subTest(length=length), self.assertRaises(FormatError):
                inspect_cgfx(bytes(data[:length]))

    def test_bad_sizes_counts_offsets_and_payloads_rejected(self):
        # Each mutation targets an independent invariant of the fixed fixture.
        for offset, value in [(12, 1151), (16, 3), (24, 7), (24, 0xffffffff),
                              (28, 65537), (32, 0), (32, 0x7fffffff),
                              (160, 43), (164, 2), (192, 0), (196, 0xffffffff),
                              (524, 0xffff0000), (512 + 0x44, 0xffffffff),
                              (512 + 0x48, 900 - (512 + 0x48)),
                              (512 + 0x48, 1140 - (512 + 0x48))]:
            data = fixture()
            put32(data, offset, value)
            with self.subTest(offset=offset, value=value), self.assertRaises(FormatError):
                inspect_cgfx(bytes(data))

    def test_magic_and_endian_rejections(self):
        for offset, value in [(0, b"BCH\0"), (4, b"\xfe\xff\x14\0"),
                              (20, b"JUNK"), (156, b"JUNK"),
                              (260, b"TXOB"), (516, b"CMDL"), (1024, b"DATA")]:
            data = fixture()
            data[offset:offset + len(value)] = value
            with self.subTest(offset=offset), self.assertRaises(FormatError):
                inspect_cgfx(bytes(data))

    def test_names_must_terminate_inside_data(self):
        data = fixture()
        put32(data, 192, 1000 - 192)
        data[1000:1024] = b"x" * 24
        with self.assertRaisesRegex(FormatError, "unterminated"):
            inspect_cgfx(bytes(data))

    def test_unknown_texture_variant_stays_explicit_and_unparsed(self):
        data = fixture()
        put32(data, 512, 0x20000004)
        put32(data, 512 + 0x48, 0xffffffff)
        result = inspect_cgfx(bytes(data))
        self.assertEqual(result["categories"][1]["records"][0]["texture"],
                         {"status": "unsupported TXOB variant"})

    def test_selected_garc_member_and_cli_errors(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "fixture.garc"
            original = garc(fixture())
            path.write_bytes(original)
            data, selection = load_selected(path, 0, 0)
            self.assertEqual(data, fixture())
            self.assertEqual(selection["offset"], 84)
            for entry, subentry in [(1, 0), (0, 1), (-1, 0), (0, 32)]:
                with self.subTest(entry=entry, subentry=subentry), self.assertRaises(FormatError):
                    load_selected(path, entry, subentry)
            command = [sys.executable, str(TOOLS / "inspect_container.py")]
            result = subprocess.run(command + ["--archive", str(path), "--entry", "0"],
                                    capture_output=True, text=True, timeout=10)
            self.assertEqual(result.returncode, 0, result.stderr)
            self.assertEqual(json.loads(result.stdout)["container"], "CGFX")
            for args in [["--archive", str(path)], ["--archive", str(path), "--entry", "9"],
                         ["--file", str(path)], ["--file", str(path), "--entry", "0"]]:
                result = subprocess.run(command + args, capture_output=True, text=True, timeout=10)
                self.assertNotEqual(result.returncode, 0)
                self.assertNotIn("Traceback", result.stderr)
            self.assertEqual(path.read_bytes(), original)


if __name__ == "__main__":
    unittest.main()
