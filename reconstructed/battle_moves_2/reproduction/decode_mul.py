#!/usr/bin/env python3
"""The battle_power decoder extended by one form: MUL (A32, `mul rd, rm, rs`).

decode_arm.py (frozen in reconstructed/battle_power) refuses multiplies. This
wrapper adds MUL/MULS with Ghidra's operand order and spelling and refuses
every other multiply form (MLA, long multiplies, halfword multiplies).
verify_rea.py checks it against every REA-listed MUL whose bytes are retained.
"""
import sys
from pathlib import Path

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'battle_power/reproduction'))
from decode_arm import CONDITIONS, decode as decode_base, register  # noqa: E402


def decode(address, word):
    """Ghidra-style text for one instruction word."""
    if (word & 0x0fc000f0) == 0x00000090 and (word >> 28) != 0xf:
        if word >> 12 & 0xf:
            raise ValueError('MUL with a nonzero Rn field')
        s = 's' if word >> 20 & 1 else ''
        rd, rs, rm = word >> 16 & 0xf, word >> 8 & 0xf, word & 0xf
        if 15 in (rd, rs, rm):
            raise ValueError('MUL with pc')
        return 'mul%s%s %s, %s, %s' % (s, CONDITIONS[word >> 28], register(rd), register(rm), register(rs))
    return decode_base(address, word)
