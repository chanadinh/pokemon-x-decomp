#!/usr/bin/env python3
"""A small ARM (A32) decoder for the instruction forms of the battle handlers.

It prints Ghidra's spelling as returned by REA (for example `cpy` for a plain
register move, `stmdb sp!,{r4,lr}`, `ldr r1, [0x108c131c]`) and is used only
where Ghidra created no function. verify_rea.py checks that it reproduces the
REA listings of every handler REA did list, and refuses any form it does not
know.
"""
CONDITIONS = ['eq', 'ne', 'cs', 'cc', 'mi', 'pl', 'vs', 'vc', 'hi', 'ls', 'ge', 'lt', 'gt', 'le', '', 'nv']
OPCODES = ['and', 'eor', 'sub', 'rsb', 'add', 'adc', 'sbc', 'rsc', 'tst', 'teq', 'cmp', 'cmn', 'orr', 'mov', 'bic', 'mvn']
SHIFTS = ['lsl', 'lsr', 'asr', 'ror']


def register(n):
    return {13: 'sp', 14: 'lr', 15: 'pc'}.get(n, 'r%d' % n)


def operand2(word):
    if word >> 25 & 1:
        rotate, imm = (word >> 8 & 0xf) * 2, word & 0xff
        return '#0x%x' % (((imm >> rotate) | (imm << (32 - rotate))) & 0xffffffff if rotate else imm)
    rm, kind = register(word & 0xf), SHIFTS[word >> 5 & 3]
    if word >> 4 & 1:
        if word >> 7 & 1:
            raise ValueError('not a data-processing operand')
        return '%s, %s %s' % (rm, kind, register(word >> 8 & 0xf))
    amount = word >> 7 & 0x1f
    if amount == 0 and kind == 'lsl':
        return rm
    if amount == 0:
        raise ValueError('unsupported shift')
    return '%s, %s #0x%x' % (rm, kind, amount)


def decode(address, word):
    """Ghidra-style text for one instruction word."""
    cond = CONDITIONS[word >> 28]
    if word == 0xe320f000:
        return 'nop'
    if cond == 'nv':
        raise ValueError('unconditional extension space at %#x' % address)
    if (word & 0x0fff03f0) == 0x06ff0070:
        return 'uxth%s %s, %s' % (cond, register(word >> 12 & 0xf), register(word & 0xf))
    kind = word >> 25 & 7
    if kind == 5:
        offset = word & 0xffffff
        offset = offset - (1 << 24) if offset & 0x800000 else offset
        return '%s%s 0x%x' % ('bl' if word >> 24 & 1 else 'b', cond, (address + 8 + 4 * offset) & 0xffffffff)
    if kind == 4:
        p, u, s, w, load = (word >> 24 & 1, word >> 23 & 1, word >> 22 & 1, word >> 21 & 1, word >> 20 & 1)
        if s:
            raise ValueError('unsupported block transfer')
        mode = ('d' if not u else 'i') + ('b' if p else 'a')
        regs = ','.join(register(n) for n in range(16) if word >> n & 1)
        return '%s%s%s %s%s,{%s}' % ('ldm' if load else 'stm', cond, mode, register(word >> 16 & 0xf), '!' if w else '', regs)
    if kind in (2, 3):
        if kind == 3 and word >> 4 & 1:
            raise ValueError('media instruction at %#x' % address)
        p, u, b, w, load = (word >> 24 & 1, word >> 23 & 1, word >> 22 & 1, word >> 21 & 1, word >> 20 & 1)
        name = ('ldr' if load else 'str') + ('b' if b else '') + cond
        rn, rd = word >> 16 & 0xf, register(word >> 12 & 0xf)
        if not p or w:
            raise ValueError('unsupported indexing at %#x' % address)
        if kind == 2:
            imm = word & 0xfff
            if rn == 15:
                return '%s %s, [0x%x]' % (name, rd, (address + 8 + (imm if u else -imm)) & 0xffffffff)
            return '%s %s, [%s,#%s0x%x]' % (name, rd, register(rn), '' if u else '-', imm)
        rm, amount = register(word & 0xf), word >> 7 & 0x1f
        if not u or word >> 5 & 3 or (word >> 4 & 1):
            raise ValueError('unsupported register offset at %#x' % address)
        return '%s %s, [%s,%s%s]' % (name, rd, register(rn), rm, ',lsl #0x%x' % amount if amount else '')
    if kind in (0, 1):
        if kind == 0 and (word >> 4 & 0x9) == 0x9 and (word >> 7 & 1):
            sh = word >> 5 & 3
            if sh == 0:
                raise ValueError('multiply forms are not supported at %#x' % address)
            load, imm_form = word >> 20 & 1, word >> 22 & 1
            if not (word >> 24 & 1) or (word >> 21 & 1) or not imm_form:
                raise ValueError('unsupported halfword transfer at %#x' % address)
            imm = (word >> 4 & 0xf0) | (word & 0xf)
            rd, base = word >> 12 & 0xf, register(word >> 16 & 0xf)
            sign = '' if word >> 23 & 1 else '-'
            if not load and sh in (2, 3):
                # ldrd / strd: a register pair.
                return '%s%s %s, %s, [%s,#%s0x%x]' % ('ldrd' if sh == 2 else 'strd', cond, register(rd), register(rd + 1),
                                                    base, sign, imm)
            name = ('ldr' if load else 'str') + {1: 'h', 2: 'sb', 3: 'sh'}[sh] + cond
            return '%s %s, [%s,#%s0x%x]' % (name, register(rd), base, sign, imm)
        opcode, s = word >> 21 & 0xf, word >> 20 & 1
        op = OPCODES[opcode]
        rn, rd = register(word >> 16 & 0xf), register(word >> 12 & 0xf)
        if 8 <= opcode <= 11:
            if not s:
                raise ValueError('miscellaneous instruction at %#x' % address)
            return '%s%s %s, %s' % (op, cond, rn, operand2(word))
        text = operand2(word)
        if opcode == 13 and not s and not word >> 25 & 1 and ',' not in text:
            return 'cpy%s %s, %s' % (cond, rd, text)
        suffix = cond + ('s' if s else '')
        if opcode in (13, 15):
            return '%s%s %s, %s' % (op, suffix, rd, text)
        return '%s%s %s, %s, %s' % (op, suffix, rd, rn, text)
    raise ValueError('unsupported instruction %#010x at %#x' % (word, address))
