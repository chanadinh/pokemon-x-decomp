1086fe6c  000052e3  cmp r2, #0x0
1086fe70  04402de5  str r4, [sp,#-0x4]!
1086fe74  1800000a  beq 0x1086fedc
1086fe78  013002e2  and r3, r2, #0x1
1086fe7c  000053e3  cmp r3, #0x0
1086fe80  00c0a0e3  mov r12, #0x0
1086fe84  0600009a  bls 0x1086fea4
1086fe88  8c4081e0  add r4, r1, r12, lsl #0x1
1086fe8c  b040d4e1  ldrh r4, [r4,#0x0]
1086fe90  000054e1  cmp r4, r0
1086fe94  0a00000a  beq 0x1086fec4
1086fe98  01c08ce2  add r12, r12, #0x1
1086fe9c  0c0053e1  cmp r3, r12
1086fea0  f8ffff8a  bhi 0x1086fe88
1086fea4  030052e1  cmp r2, r3
1086fea8  0b00009a  bls 0x1086fedc
1086feac  83c081e0  add r12, r1, r3, lsl #0x1
1086feb0  b040dce1  ldrh r4, [r12,#0x0]
1086feb4  000054e1  cmp r4, r0
1086feb8  b2c0dc11  ldrhne r12, [r12,#0x2]
1086febc  00005c11  cmpne r12, r0
1086fec0  0200001a  bne 0x1086fed0
1086fec4  04409de4  ldr r4, [sp],#0x4
1086fec8  0100a0e3  mov r0, #0x1
1086fecc  1eff2fe1  bx lr
1086fed0  023083e2  add r3, r3, #0x2
1086fed4  030052e1  cmp r2, r3
1086fed8  f3ffff8a  bhi 0x1086feac
1086fedc  04409de4  ldr r4, [sp],#0x4
1086fee0  0000a0e3  mov r0, #0x0
1086fee4  1eff2fe1  bx lr
