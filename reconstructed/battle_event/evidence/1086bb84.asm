1086bb84  58009fe5  ldr r0, [0x1086bbe4]
1086bb88  001090e5  ldr r1, [r0,#0x0]
1086bb8c  600051e3  cmp r1, #0x60
1086bb90  0700002a  bcs 0x1086bbb4
1086bb94  812080e0  add r2, r0, r1, lsl #0x1
1086bb98  b420d2e1  ldrh r2, [r2,#0x4]
1086bb9c  000052e3  cmp r2, #0x0
1086bba0  0300000a  beq 0x1086bbb4
1086bba4  011081e2  add r1, r1, #0x1
1086bba8  600051e3  cmp r1, #0x60
1086bbac  001080e5  str r1, [r0,#0x0]
1086bbb0  f7ffff3a  bcc 0x1086bb94
1086bbb4  5f0051e3  cmp r1, #0x5f
1086bbb8  0500002a  bcs 0x1086bbd4
1086bbbc  013081e2  add r3, r1, #0x1
1086bbc0  811080e0  add r1, r0, r1, lsl #0x1
1086bbc4  0120a0e3  mov r2, #0x1
1086bbc8  003080e5  str r3, [r0,#0x0]
1086bbcc  b420c1e1  strh r2, [r1,#0x4]
1086bbd0  1eff2fe1  bx lr
1086bbd4  0020a0e3  mov r2, #0x0
1086bbd8  0210a0e1  cpy r1, r2
1086bbdc  0200a0e1  cpy r0, r2
1086bbe0  ef51feea  b 0x108003a4
