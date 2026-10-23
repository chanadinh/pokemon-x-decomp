#!/usr/bin/env python3
"""Inventory local GARC v4 metadata and optionally copy selected members.

No game input is supplied by this script. It reads allocation indexes, four
header bytes and at most forty footer bytes per member, not entire archives.
Only explicitly selected small archives are hashed in full. LZ payloads are
identified by header only; use the native viewer to validate/decode them.
"""

import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import struct
import subprocess


MAX_INDEX = 8 * 1024 * 1024
MAX_MEMBERS = 262144
MAX_SAMPLE = 64 * 1024 * 1024


def u32(data, offset):
    return struct.unpack_from("<I", data, offset)[0]


def read_exact(handle, length):
    data = handle.read(length)
    if len(data) != length:
        raise ValueError("truncated input")
    return data


def archive_index(handle, file_size):
    header = read_exact(handle, 28)
    if header[:4] != b"CRAG":
        raise ValueError("not a GARC archive")
    if (u32(header, 4), header[8:12], u32(header, 12)) != (28, b"\xff\xfe\x00\x04", 4):
        raise ValueError("only little-endian v4 with four sections is supported")
    offset = u32(header, 16)
    if u32(header, 20) != file_size or not 64 <= offset <= min(file_size, MAX_INDEX):
        raise ValueError("invalid or over-budget header sizes")
    data = header + read_exact(handle, offset - 28)
    fato_size = u32(data, 32)
    entries = int.from_bytes(data[36:38], "little")
    fatb = 28 + fato_size
    if data[28:32] != b"OTAF" or fato_size != 12 + 4 * entries or fatb + 12 > offset:
        raise ValueError("invalid FATO")
    fatb_size, count = struct.unpack_from("<II", data, fatb + 4)
    fimb = fatb + fatb_size
    if data[fatb:fatb + 4] != b"BTAF" or fatb_size < 12 or count > MAX_MEMBERS:
        raise ValueError("invalid FATB")
    if fimb + 12 != offset or data[fimb:fimb + 4] != b"BMIF":
        raise ValueError("invalid FIMB/data offset")
    if u32(data, fimb + 4) != 12 or u32(data, fimb + 8) != file_size - offset:
        raise ValueError("invalid data extent")
    members = []
    cursor = fatb + 12
    for entry in range(entries):
        record = fatb + 12 + u32(data, 40 + entry * 4)
        if record != cursor or cursor + 4 > fimb:
            raise ValueError("nonpacked/truncated allocation records")
        mask = u32(data, cursor)
        cursor += 4
        for subentry in range(32):
            if not (mask >> subentry) & 1:
                continue
            if cursor + 12 > fimb or len(members) == count:
                raise ValueError("truncated/excess member records")
            start, end, size = struct.unpack_from("<III", data, cursor)
            cursor += 12
            if not start <= end <= file_size - offset or size > end - start:
                raise ValueError("invalid member extent")
            members.append({"entry": entry, "subentry": subentry,
                            "offset": offset + start, "size": size})
    if len(members) != count or cursor != fimb:
        raise ValueError("member count/record size mismatch")
    return offset, entries, members, hashlib.sha256(data).hexdigest()


def footer_fields(footer):
    return {
        "footer_hex": footer.hex(),
        "declared_file_size": u32(footer, 12),
        "width": int.from_bytes(footer[28:30], "little"),
        "height": int.from_bytes(footer[30:32], "little"),
        "format_word": u32(footer, 32),
        "declared_image_data_size": u32(footer, 36),
    }


def hash_small_archive(path):
    if path.stat().st_size > MAX_SAMPLE:
        return None
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--romfs", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path,
                        help="new report directory outside the original RomFS tree")
    parser.add_argument("--validate-native", type=Path,
                        help="run the compiled check_garc tool on the surveyed archive indexes")
    parser.add_argument("--sample", action="append", default=[], metavar="ARCHIVE:ENTRY:SUBENTRY",
                        help="copy/hash this selected member; hashes its archive if at most 64 MiB")
    args = parser.parse_args()
    root = args.romfs.resolve()
    if not (root / "a").is_dir():
        parser.error("RomFS root must contain an a/ directory")
    destination = args.output.resolve()
    if destination == root or destination.is_relative_to(root):
        parser.error("output directory must be outside the original RomFS tree")
    if args.output.exists() or args.output.is_symlink():
        parser.error("output directory already exists; choose a new directory to preserve previous reports")
    if args.validate_native is not None and not args.validate_native.is_file():
        parser.error("native validator does not exist; compile native/tools/check_garc.c first")
    selected = set()
    for text in args.sample:
        try:
            relative, entry, subentry = text.rsplit(":", 2)
            path = Path(relative)
            if path.is_absolute() or ".." in path.parts:
                raise ValueError("unsafe relative path")
            selected.add((path.as_posix(), int(entry), int(subentry)))
        except ValueError:
            parser.error("samples must be relative ARCHIVE:ENTRY:SUBENTRY selectors")
    args.output.mkdir(parents=True, exist_ok=False)
    archives, candidates, samples, errors = [], [], [], []
    matched = set()
    for path in sorted((root / "a").rglob("*")):
        if not path.is_file():
            continue
        if not path.resolve().is_relative_to(root):
            errors.append({"archive": str(path.relative_to(root)), "error": "symlink outside root"})
            continue
        relative = path.relative_to(root).as_posix()
        try:
            with path.open("rb") as handle:
                offset, entries, members, index_hash = archive_index(handle, path.stat().st_size)
                headers = Counter()
                tails = standalone = 0
                for member in members:
                    handle.seek(member["offset"])
                    head = read_exact(handle, min(4, member["size"]))
                    headers[head.hex()] += 1
                    if member["size"] >= 40:
                        handle.seek(member["offset"] + member["size"] - 40)
                        footer = read_exact(handle, 40)
                        if footer[:4] == b"CLIM":
                            candidate = {"archive": relative, **member, **footer_fields(footer)}
                            candidate["standalone_size_match"] = candidate["declared_file_size"] == member["size"]
                            candidates.append(candidate)
                            tails += 1
                            standalone += candidate["standalone_size_match"]
                    selector = (relative, member["entry"], member["subentry"])
                    if selector in selected:
                        if member["size"] > MAX_SAMPLE:
                            raise ValueError("selected member exceeds 64 MiB")
                        handle.seek(member["offset"])
                        payload = read_exact(handle, member["size"])
                        sample_name = relative.replace("/", "_") + f'_{member["entry"]}_{member["subentry"]}.bin'
                        (args.output / sample_name).write_bytes(payload)
                        samples.append({"archive": relative, **member, "header_hex": head.hex(),
                                        "member_sha256": hashlib.sha256(payload).hexdigest(),
                                        "archive_sha256": hash_small_archive(path),
                                        "sample_file": sample_name})
                        matched.add(selector)
                archives.append({"archive": relative, "size": path.stat().st_size,
                                 "index_bytes": offset, "index_sha256": index_hash,
                                 "entries": entries, "members": len(members),
                                 "clim_tail_candidates": tails, "standalone_size_matches": standalone,
                                 "member_header_counts": dict(headers)})
        except (OSError, ValueError, struct.error) as error:
            errors.append({"archive": relative, "error": str(error)})
    native_validation = None
    if args.validate_native is not None:
        try:
            process = subprocess.run([str(args.validate_native.resolve())] +
                                     [str(root / archive["archive"]) for archive in archives],
                                     capture_output=True, text=True, check=False)
            native_validation = {"exit_code": process.returncode,
                                 "stdout": process.stdout, "stderr": process.stderr}
        except OSError as error:
            native_validation = {"exit_code": 127, "stdout": "", "stderr": str(error)}
    report = {"method": "Bounded allocation indexes and member headers/footers; selected samples only copied/hashed.",
              "limitations": ["CLIM footer and size agreement do not prove decoded image validity or asset role.",
                              "Compressed member headers are not decompression validation.",
                              "Unselected payloads and large archive containers are not hashed."],
              "archives": archives, "candidates": candidates, "selected_samples": samples,
              "native_validation": native_validation,
              "errors": errors, "missing_samples": [list(x) for x in sorted(selected - matched)]}
    (args.output / "archive-survey.json").write_text(json.dumps(report, indent=2) + "\n")
    print(f'{len(archives)} archives, {sum(x["members"] for x in archives)} members; '
          f'{len(candidates)} CLIM tails, {sum(x["standalone_size_match"] for x in candidates)} size matches; '
          f'{len(samples)} selected samples; {len(errors)} errors')
    return 1 if errors or selected - matched or (native_validation and native_validation["exit_code"]) else 0


if __name__ == "__main__":
    raise SystemExit(main())
