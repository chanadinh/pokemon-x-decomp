"""Checks the instruction and data words behind the battle's lights.

1. DllBattle.cro (RomFS): the battle looks up its Pokémon lights by name.
   At 0x38f54 it loads a literal (0x393d0) that the CRO's internal relocation
   table (header +0x128) points at rodata 0xd8c8c; 0x38f60 adds 0xc8 to it,
   "light_poke_1" (0xd8d54); the loop copies the 16-byte name, adds the loop
   index to its byte 11 ('1' + k), looks the name up in the scene and stores
   the result at +0xb0 + 4k, for k = 0..5 (cmp #6 at 0x38fac); then +0xd8,
   "light_eff_obj", stored at +0xc8.
2. code.bin: the lighting config written for a material (0x2e8f88..0x2e9090)
   takes a flags word reached through the material object's relative
   pointer at +0x288 (+0x10); its bit 5 (0x2e9030..0x2e9074) leaves the
   reflection LUTs R, G, B enabled (config1 bits 20..22 = 0) or disables
   all three (7 << 20). The same bit is computed at 0x2e8f54..0x2e8f6c,
   stored at [sp, #4] and passed as the fourth argument (r3, 0x2e9234) of
   the color loop function 0x302edc (call at 0x2e9244), whose branch at
   0x3031d8 then writes specular1 as the light's own color instead of the
   product with the material's specular1 (0x3031fc..0x303208).
3. code.bin: the DMP uniform table names dmp_LightEnv.lutEnabledRefl
   (0x59f7d0) as GL_BOOL uniform 0xea (0x56bf90); 0x3e0fe4 maps uniforms
   0xe7..0xea (clampHighlights, lutEnabledD0, lutEnabledD1, lutEnabledRefl)
   to 0..3.
Reads only the two binaries. Run with python3 -I.
"""
import argparse
import json
from pathlib import Path
import struct

CODE_BASE = 0x00100000
CODE = [
    (0x302edc, 0xe92d4fff, 'push {r0-r11, lr}: the fourth argument r3 lands at [sp, #0x30]'),
    (0x3030c4, 0xe3510000, 'cmp r1 (= [sp, #0x30]), #0'),
    (0x3031d8, 0x0a000007, 'beq 0x3031fc: argument clear -> product'),
    (0x3031dc, 0xe1a0e80b, 'argument set: specular1 fields from the light color (fp) alone'),
    (0x3031fc, 0xe59d1020, 'ldr r1, [sp, #0x20]: material'),
    (0x303208, 0xe591e0c0, 'ldr lr, [material, #0xc0]: material specular1'),
    (0x2e8f3c, 0xe5b10288, 'ldr r0, [r1, #0x288]!: relative pointer'),
    (0x2e8f48, 0xe2800010, 'add r0, r0, #0x10: the flags word'),
    (0x2e8f50, 0xe5900000, 'ldr r0, [r0]'),
    (0x2e8f54, 0xe3a01020, 'mov r1, #0x20'),
    (0x2e8f58, 0xe1d10000, 'bics r0, r1, r0'),
    (0x2e8f60, 0x03a00001, 'moveq r0, #1: bit 5 set'),
    (0x2e8f64, 0x13a00000, 'movne r0, #0'),
    (0x2e8f6c, 0xe58d0004, 'str r0, [sp, #4]'),
    (0x2e9030, 0xe3a00020, 'mov r0, #0x20'),
    (0x2e9034, 0xe1d0000a, 'bics r0, r0, sl (flags)'),
    (0x2e9040, 0x03a03001, 'moveq r3, #1: bit 5 set'),
    (0x2e905c, 0xe3530000, 'cmp r3, #0'),
    (0x2e9068, 0x13a01000, 'movne r1, #0: reflection LUTs enabled'),
    (0x2e906c, 0x03a01007, 'moveq r1, #7: R, G, B disabled'),
    (0x2e9074, 0xe1800a01, 'orr r0, r0, r1, lsl #20: config1 bits 20..22'),
    (0x2e9234, 0xe59d3004, 'ldr r3, [sp, #4]: the bit as the fourth argument'),
    (0x2e9244, 0xeb006724, 'bl 0x302edc: the color loop'),
    (0x3e0fe4, 0xe1a00c20, 'lsr r0, r0, #24: uniform id'),
    (0x3e1000, 0xe35000ea, 'cmp r0, #0xea: lutEnabledRefl'),
    (0x3e1010, 0x13a00003, 'movne r0, #3'),
    (0x56bf90, 0x0059f7d0, 'uniform table: name pointer'),
    (0x56bf94, 0x000000ea, 'uniform id 0xea'),
]
CRO = [
    (0x38f54, 0xe59f6474, 'ldr r6, [pc, #0x474]: literal at 0x393d0'),
    (0x38f60, 0xe28600c8, 'add r0, r6, #0xc8: "light_poke_1"'),
    (0x38f68, 0xe890000f, 'ldm r0, {r0-r3}: 16 bytes of the name'),
    (0x38f74, 0xe5dd0013, 'ldrb r0, [sp, #0x13]: byte 11 of the copy'),
    (0x38f78, 0xe0800005, 'add r0, r0, r5: + k'),
    (0x38f7c, 0xe5cd0013, 'strb r0, [sp, #0x13]'),
    (0x38fac, 0xe3550006, 'cmp r5, #6: six lights'),
    (0x38fb0, 0xe58010b0, 'str r1, [r0, #0xb0]: light k'),
    (0x38fb8, 0xe28660d8, 'add r6, r6, #0xd8: "light_eff_obj"'),
    (0x38ff0, 0xe58400c8, 'str r0, [r4, #0xc8]'),
]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--code-bin', type=Path, required=True)
    ap.add_argument('--cro', type=Path, required=True)
    args = ap.parse_args()
    code, cro = args.code_bin.read_bytes(), args.cro.read_bytes()
    word = lambda data, at: struct.unpack_from('<I', data, at)[0]
    bad = [hex(a) for a, w, _ in CODE if word(code, a - CODE_BASE) != w]
    bad += ['cro:' + hex(a) for a, w, _ in CRO if word(cro, a) != w]
    assert code[0x59f7d0 - CODE_BASE:].split(b'\0')[0] == b'dmp_LightEnv.lutEnabledRefl'
    assert cro[0x80:0x84] == b'CRO0'
    segments = [struct.unpack_from('<III', cro, word(cro, 0xc8) + 12 * i) for i in range(word(cro, 0xcc))]
    table, count = word(cro, 0x128), word(cro, 0x12c)
    literal = None
    for k in range(count):
        target, kind, segment, _, _, addend = struct.unpack_from('<IBBBBI', cro, table + 12 * k)
        if segments[target & 15][0] + (target >> 4) == 0x393d0:
            literal = (kind, segments[segment][0] + addend)
    assert literal == (2, 0xd8c8c), literal
    names = {off: cro[0xd8c8c + off:].split(b'\0')[0].decode() for off in (0xc8, 0xd8)}
    assert names == {0xc8: 'light_poke_1', 0xd8: 'light_eff_obj'}, names
    result = {'code_words_checked': len(CODE), 'cro_words_checked': len(CRO), 'mismatches': bad,
              'literal_0x393d0': hex(literal[1]), 'names': {hex(k): v for k, v in names.items()}}
    print(json.dumps(result, indent=2))
    assert not bad


if __name__ == '__main__':
    main()
