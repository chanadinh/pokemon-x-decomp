#!/usr/bin/env python3
"""Read bounded CGFX resource indexes, without rendering or exporting payloads.

Accept a standalone little-endian CGFX, or one uncompressed member selected
from a GARC v4 archive. JSON goes to stdout; errors go to stderr. Offsets in
the report are relative to the selected CGFX, not the enclosing archive.
"""

import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys

from survey_assets import archive_index, read_exact


MAX_BYTES = 64 * 1024 * 1024
MAX_ENTRIES = 65536
MAX_NAME = 1024
CATEGORIES = (
    "models", "textures", "lookup_tables", "materials", "shaders", "cameras",
    "lights", "fogs", "scenes", "skeletal_animations", "material_animations",
    "visibility_animations", "camera_animations", "light_animations",
    "emitters", "slot_15",
)
TEXTURE_FORMATS = (
    "RGBA8", "RGB8", "RGBA5551", "RGB565", "RGBA4", "LA8", "HILO8",
    "L8", "A8", "LA4", "L4", "A4", "ETC1", "ETC1A4",
)


class FormatError(ValueError):
    """An unsupported, inconsistent, truncated, or over-budget container."""


class Reader:
    def __init__(self, data):
        self.data = data
        self.data_start = 0
        self.data_end = len(data)

    def span(self, offset, size, *, in_data=False):
        lower, upper = (self.data_start, self.data_end) if in_data else (0, len(self.data))
        if offset < lower or size < 0 or offset > upper or size > upper - offset:
            raise FormatError(f"extent outside {'DATA' if in_data else 'file'} at 0x{offset:x}")

    def u32(self, offset):
        self.span(offset, 4)
        return struct.unpack_from("<I", self.data, offset)[0]

    def relative(self, offset, size=1, *, nullable=False, in_data=True):
        value = self.u32(offset)
        if value == 0:
            if nullable:
                return None
            raise FormatError(f"missing relative pointer at 0x{offset:x}")
        displacement = value if value < 0x80000000 else value - 0x100000000
        target = offset + displacement
        self.span(target, size, in_data=in_data)
        return target

    def name(self, field):
        offset = self.relative(field)
        stop = min(self.data_end, offset + MAX_NAME + 1)
        end = self.data.find(b"\0", offset, stop)
        if end == -1:
            raise FormatError(f"unterminated or over-budget name at 0x{offset:x}")
        raw = self.data[offset:end]
        return {"name": raw.decode("utf-8", errors="replace"),
                "name_hex": raw.hex(), "name_offset": offset}

    def dictionary(self, field, count):
        offset = self.relative(field, 28, nullable=count == 0)
        if offset is None:
            return None, []
        if self.data[offset:offset + 4] != b"DICT":
            raise FormatError(f"missing DICT at 0x{offset:x}")
        size, actual = self.u32(offset + 4), self.u32(offset + 8)
        if actual != count or size != 28 + count * 16:
            raise FormatError(f"DICT count/size mismatch at 0x{offset:x}")
        self.span(offset, size, in_data=True)
        records = []
        for index in range(count):
            record = offset + 28 + index * 16
            target = self.relative(record + 12, 8)
            records.append({"index": index, "record_offset": record,
                            **self.name(record + 8), "object_offset": target,
                            "object_magic": self.data[target + 4:target + 8].decode("ascii", errors="replace"),
                            "object_flags": self.u32(target)})
        return offset, records


def texture_metadata(reader, record, image_blocks):
    offset = record["object_offset"]
    if record["object_magic"] != "TXOB":
        raise FormatError(f"texture index points to non-TXOB at 0x{offset:x}")
    # The inline image form observed locally and described by the CGFX loader.
    # Reference/cube/dynamic variants remain explicitly unparsed.
    if record["object_flags"] != 0x20000011:
        return {"status": "unsupported TXOB variant"}
    reader.span(offset, 0x3c, in_data=True)
    descriptor = reader.relative(offset + 0x38, 32)
    size = reader.u32(descriptor + 8)
    target = reader.relative(descriptor + 12, size, in_data=False)
    if not size or not any(start <= target and size <= end - target
                           for start, end in image_blocks if target <= end):
        raise FormatError(f"texture payload outside IMAG at 0x{target:x}")
    fmt = reader.u32(offset + 0x34)
    return {
        "status": "indexed", **reader.name(offset + 12),
        "image_offset": descriptor,
        "height": reader.u32(offset + 0x18), "width": reader.u32(offset + 0x1c),
        "mipmap_levels": reader.u32(offset + 0x28),
        "format": fmt,
        "format_name": TEXTURE_FORMATS[fmt] if fmt < len(TEXTURE_FORMATS) else "unknown",
        "stored_height": reader.u32(descriptor),
        "stored_width": reader.u32(descriptor + 4),
        "data_offset": target, "data_size": size,
        "bits_per_pixel": reader.u32(descriptor + 20),
    }


def model_metadata(reader, record):
    offset = record["object_offset"]
    if record["object_magic"] != "CMDL":
        raise FormatError(f"model index points to non-CMDL at 0x{offset:x}")
    reader.span(offset, 0xe0, in_data=True)
    return {**reader.name(offset + 12),
            "has_skeleton_flag": bool(record["object_flags"] & 0x80),
            "object_count": reader.u32(offset + 0xb4),
            "material_count": reader.u32(offset + 0xbc),
            "shape_count": reader.u32(offset + 0xc4),
            "object_node_count": reader.u32(offset + 0xcc)}


def inspect_cgfx(data):
    """Return resource-index metadata; reject unsafe or unsupported layouts."""
    if len(data) > MAX_BYTES:
        raise FormatError("CGFX exceeds 64 MiB budget")
    reader = Reader(data)
    reader.span(0, 20)
    if data[:4] != b"CGFX":
        raise FormatError("not a standalone CGFX (compressed and other containers are unsupported)")
    if data[4:8] != b"\xff\xfe\x14\x00":
        raise FormatError("only little-endian CGFX with a 20-byte header is supported")
    if reader.u32(12) != len(data):
        raise FormatError("CGFX declared file size differs from selected input")
    count = reader.u32(16)
    if count not in (1, 2):
        raise FormatError("only DATA with optional IMAG is supported")
    blocks, image_blocks = [], []
    cursor = 20
    for index in range(count):
        reader.span(cursor, 8)
        magic, size = data[cursor:cursor + 4], reader.u32(cursor + 4)
        if size < 8 or magic != (b"DATA" if index == 0 else b"IMAG"):
            raise FormatError(f"unsupported block or size at 0x{cursor:x}")
        reader.span(cursor, size)
        blocks.append({"magic": magic.decode("ascii"), "offset": cursor, "size": size})
        if magic == b"DATA":
            reader.data_start, reader.data_end = cursor + 8, cursor + size
            reader.span(cursor + 8, 16 * 8, in_data=True)
        else:
            image_blocks.append((cursor + 8, cursor + size))
        cursor += size
    if cursor != len(data):
        raise FormatError("unindexed trailing bytes after CGFX blocks")
    categories, total = [], 0
    for slot, label in enumerate(CATEGORIES):
        field = 28 + slot * 8
        count = reader.u32(field)
        total += count
        if total > MAX_ENTRIES:
            raise FormatError("resource index exceeds 65536 entries")
        offset, records = reader.dictionary(field + 4, count)
        for record in records:
            if slot == 0:
                record["model"] = model_metadata(reader, record)
            elif slot == 1:
                record["texture"] = texture_metadata(reader, record, image_blocks)
        if offset is not None:
            categories.append({"slot": slot, "category": label, "count": count,
                               "dictionary_offset": offset, "records": records})
    return {"container": "CGFX", "size": len(data),
            "sha256": hashlib.sha256(data).hexdigest(), "revision": reader.u32(8),
            "blocks": blocks, "categories": categories,
            "limitations": "Resource index only; no geometry, animation, material, map, or texture decoding. "
                           "Unknown TXOB variants remain unparsed; texture dimensions/formats are declared metadata."}


def load_selected(path, entry=None, subentry=0):
    """Read only a bounded file/member; never copy the enclosing archive."""
    with path.open("rb") as handle:
        size = path.stat().st_size
        if entry is None:
            if size > MAX_BYTES:
                raise FormatError("input exceeds 64 MiB budget")
            return read_exact(handle, size), {"file": str(path)}
        if entry < 0 or not 0 <= subentry <= 31:
            raise FormatError("entry must be nonnegative and subentry must be 0..31")
        _, _, members, index_hash = archive_index(handle, size)
        selected = next((member for member in members
                         if (member["entry"], member["subentry"]) == (entry, subentry)), None)
        if selected is None:
            raise FormatError("GARC selection does not exist")
        if selected["size"] > MAX_BYTES:
            raise FormatError("selected member exceeds 64 MiB budget")
        handle.seek(selected["offset"])
        return read_exact(handle, selected["size"]), {
            "archive": str(path), "archive_index_sha256": index_hash, **selected}


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    source = parser.add_mutually_exclusive_group(required=True)
    source.add_argument("--file", type=Path, help="standalone uncompressed CGFX")
    source.add_argument("--archive", type=Path, help="GARC archive containing uncompressed CGFX")
    parser.add_argument("--entry", type=int)
    parser.add_argument("--subentry", type=int, default=0)
    args = parser.parse_args()
    if args.archive is not None and args.entry is None:
        parser.error("--archive requires --entry")
    if args.file is not None and (args.entry is not None or args.subentry != 0):
        parser.error("member selectors require --archive")
    try:
        data, selection = load_selected(args.file or args.archive, args.entry, args.subentry)
        report = inspect_cgfx(data)
        report["selection"] = selection
        print(json.dumps(report, indent=2, ensure_ascii=True))
    except (OSError, ValueError, struct.error) as error:
        print(f"container error: {error}", file=sys.stderr)
        return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
