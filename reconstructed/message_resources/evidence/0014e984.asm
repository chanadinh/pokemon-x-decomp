0014e984  b010d0e1  ldrh r1, [r0,#0x0]
0014e988  000051e3  cmp r1, #0x0
0014e98c  0010a0e3  mov r1, #0x0
0014e990  0900000a  beq 0x0014e9bc
0014e994  b020d0e1  ldrh r2, [r0,#0x0]
0014e998  100052e3  cmp r2, #0x10
0014e99c  02008012  addne r0, r0, #0x2
0014e9a0  01108112  addne r1, r1, #0x1
0014e9a4  0600000a  beq 0x0014e9c4
0014e9a8  020b51e3  cmp r1, #0x800
0014e9ac  0200002a  bcs 0x0014e9bc
0014e9b0  b020d0e1  ldrh r2, [r0,#0x0]
0014e9b4  000052e3  cmp r2, #0x0
0014e9b8  f5ffff1a  bne 0x0014e994
0014e9bc  0100a0e1  cpy r0, r1
0014e9c0  1eff2fe1  bx lr
0014e9c4  b230d0e1  ldrh r3, [r0,#0x2]
0014e9c8  0020a0e1  cpy r2, r0
0014e9cc  023083e2  add r3, r3, #0x2
0014e9d0  830080e0  add r0, r0, r3, lsl #0x1
0014e9d4  022040e0  sub r2, r0, r2
0014e9d8  c21081e0  add r1, r1, r2, asr #0x1
0014e9dc  f1ffffea  b 0x0014e9a8
