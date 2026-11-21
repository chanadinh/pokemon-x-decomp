10873fd8  0320d0e5  ldrb r2, [r0,#0x3]
10873fdc  0830a0e3  mov r3, #0x8
10873fe0  220253e1  cmp r3, r2, lsr #0x4
10873fe4  0800009a  bls 0x1087400c
10873fe8  2232a0e1  mov r3, r2, lsr #0x4
10873fec  f020c2e3  bic r2, r2, #0xf0
10873ff0  03c2a0e1  mov r12, r3, lsl #0x4
10873ff4  10c08ce2  add r12, r12, #0x10
10873ff8  f0c00ce2  and r12, r12, #0xf0
10873ffc  033180e0  add r3, r0, r3, lsl #0x2
10874000  0c2082e1  orr r2, r2, r12
10874004  0320c0e5  strb r2, [r0,#0x3]
10874008  041083e5  str r1, [r3,#0x4]
1087400c  1eff2fe1  bx lr
