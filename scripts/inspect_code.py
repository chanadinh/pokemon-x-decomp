#!/usr/bin/env python3
"""Validate an extracted 3DS application code/ExHeader pair for Ghidra import.

Only structurally valid decompressed raw code is accepted. This does not identify
the game or prove that the two inputs came from the same original container.
Section fields and BSS placement follow the NCCH ExHeader code-set definitions
used by https://github.com/Raikaru/ghidra-ctr-loader and
https://github.com/kynex7510/3ds_ida/blob/main/ctr_loader.py .
"""

import argparse
import hashlib
import json
from pathlib import Path
import struct
import sys


PAGE_SIZE = 0x1000
ADDRESS_LIMIT = 1 << 32


class IntakeError(ValueError):
    """Input is unsupported or inconsistent with its ExHeader."""


def sha256_file(path):
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def inspect_pair(code_path, exheader_path, decompressed=False):
    """Return a JSON-compatible import manifest, raising IntakeError on failure."""
    code_path = Path(code_path).resolve()
    exheader_path = Path(exheader_path).resolve()
    if code_path == exheader_path:
        raise IntakeError("Code and ExHeader must be different files")
    header_size = exheader_path.stat().st_size
    if header_size not in (0x400, 0x800):
        raise IntakeError("ExHeader must be exactly 0x400 or 0x800 bytes")
    header = exheader_path.read_bytes()
    code_size = code_path.stat().st_size
    compressed_flag = bool(header[0x0D] & 1)
    if compressed_flag and not decompressed:
        raise IntakeError(
            "ExHeader marks .code as compressed. Supply already-decompressed "
            "code and --decompressed to acknowledge the retained flag; "
            "this tool does not decompress inputs."
        )

    sections = []
    for name, field_offset, writable, executable in (
        (".text", 0x10, False, True),
        (".rodata", 0x20, False, False),
        (".data", 0x30, True, False),
    ):
        address, pages, size = struct.unpack_from("<III", header, field_offset)
        allocation_size = pages * PAGE_SIZE
        if size > allocation_size:
            raise IntakeError(f"{name} size exceeds its ExHeader page allocation")
        if allocation_size and (not address or address % PAGE_SIZE):
            raise IntakeError(f"{name} address must be nonzero and page-aligned")
        if address + allocation_size > ADDRESS_LIMIT:
            raise IntakeError(f"{name} page allocation exceeds the 32-bit address space")
        sections.append({
            "name": name, "address": address, "size": size,
            "pages": pages, "allocation_size": allocation_size,
            "read": True, "write": writable, "execute": executable,
        })
    if sections[0]["size"] < 4:
        raise IntakeError(".text must contain at least one 4-byte ARM instruction")

    allocations = sorted(
        (s["address"], s["address"] + s["allocation_size"], s["name"])
        for s in sections if s["allocation_size"]
    )
    for previous, current in zip(allocations, allocations[1:]):
        if current[0] < previous[1]:
            raise IntakeError(f"{previous[2]} and {current[2]} page allocations overlap")

    packed_size = sum(s["size"] for s in sections)
    padded_size = sum(s["allocation_size"] for s in sections)
    if code_size == padded_size:
        layout = "page-padded"
        stride = "allocation_size"
    elif code_size == packed_size:
        layout = "packed"
        stride = "size"
    else:
        raise IntakeError(
            f"Code length {code_size:#x} does not match decompressed "
            f"page-padded ({padded_size:#x}) or packed ({packed_size:#x}) layout; "
            "input may be compressed, truncated, or paired with the wrong ExHeader"
        )

    segments = []
    file_offset = 0
    for section in sections:
        section["offset"] = file_offset
        if section["size"]:
            segments.append(dict(section))
        file_offset += section[stride]

    bss_size = struct.unpack_from("<I", header, 0x3C)[0]
    data = sections[2]
    bss_address = data["address"] + data["size"]
    if bss_size:
        if not data["address"]:
            raise IntakeError("BSS requires a nonzero .data address; layout is unsupported")
        if data["address"] % PAGE_SIZE:
            raise IntakeError("BSS requires a page-aligned .data address")
        if bss_address + bss_size > ADDRESS_LIMIT:
            raise IntakeError("BSS exceeds the 32-bit address space")
        for section in sections:
            if section["name"] != ".data" and section["allocation_size"] and (
                bss_address < section["address"] + section["allocation_size"]
                and section["address"] < bss_address + bss_size
            ):
                raise IntakeError(f"BSS overlaps {section['name']} page allocation")
        segments.append({
            "name": ".bss", "address": bss_address, "offset": None,
            "size": bss_size, "read": True, "write": True, "execute": False,
        })

    warnings = [
        "Structural checks cannot prove that code and ExHeader are from the same build.",
        "Region and game revision are not established by this manifest.",
    ]
    if compressed_flag:
        warnings.append(
            "ExHeader compression flag is set; --decompressed explicitly asserts "
            "that the supplied code was already decompressed. Length checks passed."
        )
    return {
        "schema_version": 1,
        "format": "nintendo-3ds-extracted-code",
        "code": {"path": str(code_path), "sha256": sha256_file(code_path), "size": code_size},
        "exheader": {"path": str(exheader_path), "sha256": hashlib.sha256(header).hexdigest(), "size": header_size},
        "metadata": {
            "application_name": header[:8].rstrip(b"\0").decode("ascii", errors="replace"),
            "application_name_hex": header[:8].hex(),
            "program_id": f"{struct.unpack_from('<Q', header, 0x200)[0]:016X}",
            "remaster_version": struct.unpack_from("<H", header, 0x0E)[0],
            "stack_size": struct.unpack_from("<I", header, 0x1C)[0],
            "exheader_compressed_flag": compressed_flag,
            "decompressed_asserted": bool(decompressed),
        },
        "architecture": {"family": "ARM", "variant": "ARMv6K", "bits": 32, "endian": "little"},
        "entry_point": sections[0]["address"],
        "entry_mode": "ARM",
        "layout": layout,
        "expected_code_sizes": {"page_padded": padded_size, "packed": packed_size},
        "sections": sections,
        "segments": segments,
        "warnings": warnings,
    }


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--code", required=True, type=Path, help="Extracted, decompressed .code or code.bin")
    parser.add_argument("--exheader", required=True, type=Path, help="Matching 0x400/0x800-byte exheader.bin")
    parser.add_argument("--output", required=True, type=Path, help="Destination JSON import manifest")
    parser.add_argument("--decompressed", action="store_true", help="Assert code was decompressed even if ExHeader retains the compression flag")
    args = parser.parse_args(argv)
    try:
        output_path = args.output.resolve()
        if output_path in (args.code.resolve(), args.exheader.resolve()):
            raise IntakeError("Output must not overwrite either input")
        manifest = inspect_pair(args.code, args.exheader, args.decompressed)
        output_path.parent.mkdir(parents=True, exist_ok=True)
        output_path.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    except (OSError, IntakeError) as exc:
        print(f"error: {exc}", file=sys.stderr)
        return 2
    print(f"Validated {manifest['layout']} code; wrote {output_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
