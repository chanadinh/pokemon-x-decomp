#!/usr/bin/env python3
"""The battle_moves_3 decoder (decode_arm plus MUL, UMULL and BX) extended by one form:
STR (word, immediate offset) pre-indexed with writeback, `str rd, [rn,#imm]!`.

Other writeback and post-indexed forms still reach the frozen decoders. verify_rea.py
checks the form against every REA-listed instruction of that form whose bytes are retained.
"""
import sys
from pathlib import Path

sys.dont_write_bytecode = True
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / 'battle_moves_3/reproduction'))
from decode_mult import CONDITIONS, decode as decode_previous, register  # noqa: E402


def decode(address, word):
    """Ghidra-style text for one instruction word."""
    # cond 010 P=1 U B=0 W=1 L=0: STR word, immediate, pre-indexed with writeback.
    if (word & 0x0f700000) == 0x05200000 and (word >> 28) != 0xf:
        rn, rd, imm = word >> 16 & 0xf, word >> 12 & 0xf, word & 0xfff
        if rn == 15 or rn == rd:
            raise ValueError('STR writeback with pc or Rn = Rd')
        sign = '' if word >> 23 & 1 else '-'
        return 'str%s %s, [%s,#%s0x%x]!' % (CONDITIONS[word >> 28], register(rd), register(rn), sign, imm)
    return decode_previous(address, word)
