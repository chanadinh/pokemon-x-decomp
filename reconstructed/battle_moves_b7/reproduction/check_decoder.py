#!/usr/bin/env python3
"""Check that this milestone's decode_arm.py differs from battle_power's only in the post-indexed byte load.

The copy next to this script extends battle_power/reproduction/decode_arm.py (the
original, never modified) with one form: `ldrb<cond> rd, [rn],#imm` (post-indexed
immediate byte load, add, no write-back flag, rn not pc). This script

 1. prints the SHA-256 of both files and checks the original's against the one
    recorded in decode_arm.diff's header (the value README.txt quotes);
 2. decodes every instruction word of every evidence listing under
    reconstructed/*/evidence/*.asm (all committed battle_moves_b*/ and
    battle_moves_N/ milestones included) with both decoders: the results, or the
    refusals, must be identical, except for words of the extended form (a few
    Ghidra-listed ones exist), which the original refuses and the copy must
    spell as the listing does (ignoring spaces);
 3. sweeps 4,000,000 pseudo-random words (fixed seed) plus every word of the
    extended form's neighbourhood: the decoders must agree on every word except
    those of the extended form, which the original refuses and the copy decodes;
 4. checks the copy against REA's own decoding of word e4d03001 at 108b8950
    (`ldrb r3,[r0],#0x1`).
"""
import hashlib
import importlib.util
import random
import sys
from pathlib import Path

sys.dont_write_bytecode = True
HERE = Path(__file__).resolve().parent
ORIGINAL_SHA256 = '7b8658ce88254bf87eece1a767521fba45b277301e56f1984eca5ed22624fcd2'


def load(path, name):
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def outcome(decoder, address, word):
    try:
        return ('ok', decoder.decode(address, word))
    except ValueError as exc:
        return ('refused', str(exc))


def extended_form(word):
    cond_nv = word >> 28 == 15
    return (not cond_nv and (word >> 25 & 7) == 2 and not word >> 24 & 1 and not word >> 21 & 1 and word >> 22 & 1
            and word >> 20 & 1 and (word >> 16 & 0xf) != 15 and word >> 23 & 1
            and word != 0xe320f000 and (word & 0x0fff03f0) != 0x06ff0070)


def main():
    root = HERE.parents[1]
    original_path = root / 'battle_power/reproduction/decode_arm.py'
    original_hash = hashlib.sha256(original_path.read_bytes()).hexdigest()
    copy_hash = hashlib.sha256((HERE / 'decode_arm.py').read_bytes()).hexdigest()
    if original_hash != ORIGINAL_SHA256:
        raise SystemExit('battle_power decode_arm.py is not the original this copy was made from')
    original, copy = load(original_path, 'original_decode_arm'), load(HERE / 'decode_arm.py', 'copy_decode_arm')
    words = 0
    listings = 0
    listed_extended = 0
    for asm in sorted(root.glob('*/evidence/*.asm')):
        listings += 1
        for line in asm.read_text().splitlines():
            address, raw, text = line.split(None, 2)
            address, word = int(address, 16), int.from_bytes(bytes.fromhex(raw), 'little')
            before, after = outcome(original, address, word), outcome(copy, address, word)
            if extended_form(word):
                # The original refuses the form; the copy must reproduce the listing of it (some listings
                # space the operands, others do not).
                if before[0] != 'refused' or after[0] != 'ok' or after[1].replace(' ', '') != text.replace(' ', ''):
                    raise SystemExit(f'extended form differs from the listing in {asm.relative_to(root)} at {address:08x}')
                listed_extended += 1
            elif before != after:
                raise SystemExit(f'decoders differ on {asm.relative_to(root)} word {word:08x} at {address:08x}')
            words += 1
    rng = random.Random(0x1b7)
    extended = 0
    sweep = [rng.getrandbits(32) for _ in range(4_000_000)]
    sweep += [(c << 28) | 0x04d00000 | (u << 23) | (rn << 16) | (rd << 12) | imm
              for c in (0, 8, 14) for u in (0, 1) for rn in (0, 3, 15) for rd in (0, 3, 15) for imm in (0, 1, 4, 0xfff)]
    for word in sweep:
        a, b = outcome(original, 0x108b8950, word), outcome(copy, 0x108b8950, word)
        if extended_form(word):
            if a[0] != 'refused' or b[0] != 'ok' or not b[1].startswith('ldrb'):
                raise SystemExit(f'extended form misbehaves on {word:08x}')
            extended += 1
        elif a != b:
            raise SystemExit(f'decoders differ outside the extended form on {word:08x}')
    reference = copy.decode(0x108b8950, 0xe4d03001)
    if reference.replace(' ', '') != 'ldrb r3,[r0],#0x1'.replace(' ', ''):
        raise SystemExit('copy disagrees with REA on e4d03001: %s' % reference)
    print(f'original sha256 {original_hash}; copy sha256 {copy_hash}')
    print(f'{words} listed words in {listings} evidence listings decode identically ({listed_extended} of the extended form, '
          f'each equal to its REA listing); '
          f'{len(sweep)} swept words agree except {extended} of the extended form; e4d03001 = "{reference}" as REA')


if __name__ == '__main__':
    main()
