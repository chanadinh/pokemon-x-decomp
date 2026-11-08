#!/usr/bin/env python3
"""The battle_moves_2 decoder (battle_power's decode_arm plus MUL) extended by UMULL and BX.

Adds `umull{s}{cond} rdlo, rdhi, rm, rs` and `bx{cond} rm` in Ghidra's
spelling. The other long multiplies (SMULL, UMLAL, SMLAL) reach the frozen
decoders and are refused there. verify_rea.py checks the forms against every
REA-listed UMULL, MUL and BX whose bytes are retained.
"""
import sys
from pathlib import Path

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'battle_moves_2/reproduction'))
from decode_mul import CONDITIONS, decode as decode_previous, register  # noqa: E402


def decode(address, word):
    """Ghidra-style text for one instruction word."""
    if (word & 0x0fe000f0) == 0x00800090 and (word >> 28) != 0xf:
        s = 's' if word >> 20 & 1 else ''
        hi, lo, rs, rm = word >> 16 & 0xf, word >> 12 & 0xf, word >> 8 & 0xf, word & 0xf
        if 15 in (hi, lo, rs, rm) or hi == lo:
            raise ValueError('UMULL with pc or equal destinations')
        return 'umull%s%s %s, %s, %s, %s' % (s, CONDITIONS[word >> 28], register(lo), register(hi), register(rm), register(rs))
    if (word & 0x0ffffff0) == 0x012fff10 and (word >> 28) != 0xf:
        return 'bx%s %s' % (CONDITIONS[word >> 28], register(word & 0xf))
    return decode_previous(address, word)
