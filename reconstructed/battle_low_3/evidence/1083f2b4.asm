1083f2b4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1083f2b8  0150a0e1  cpy r5, r1
1083f2bc  0040a0e1  cpy r4, r0
1083f2c0  78109fe5  ldr r1, [0x1083f340]
1083f2c4  78009fe5  ldr r0, [0x1083f344]
1083f2c8  ff60a0e3  mov r6, #0xff
1083f2cc  6c2091e5  ldr r2, [r1,#0x6c]
1083f2d0  000052e1  cmp r2, r0
1083f2d4  6c008115  strne r0, [r1,#0x6c]
1083f2d8  060c84e2  add r0, r4, #0x600
1083f2dc  ca0080e2  add r0, r0, #0xca
1083f2e0  0610a0e1  cpy r1, r6
1083f2e4  8bc200eb  bl 0x1086fd18
1083f2e8  0070a0e3  mov r7, #0x0
1083f2ec  c966c4e5  strb r6, [r4,#0x6c9]
1083f2f0  050184e0  add r0, r4, r5, lsl #0x2
1083f2f4  5d72c4e5  strb r7, [r4,#0x25d]
1083f2f8  c856c4e5  strb r5, [r4,#0x6c8]
1083f2fc  000190e5  ldr r0, [r0,#0x100]
1083f300  001090e5  ldr r1, [r0,#0x0]
1083f304  7c1091e5  ldr r1, [r1,#0x7c]
1083f308  31ff2fe1  blx r1
1083f30c  000050e3  cmp r0, #0x0
1083f310  0600001a  bne 0x1083f330
1083f314  c806d4e5  ldrb r0, [r4,#0x6c8]
1083f318  000184e0  add r0, r4, r0, lsl #0x2
1083f31c  000190e5  ldr r0, [r0,#0x100]
1083f320  5c01d0e5  ldrb r0, [r0,#0x15c]
1083f324  000050e3  cmp r0, #0x0
1083f328  6400a003  moveq r0, #0x64
1083f32c  0100000a  beq 0x1083f338
1083f330  0100a0e3  mov r0, #0x1
1083f334  b47684e5  str r7, [r4,#0x6b4]
1083f338  b80684e5  str r0, [r4,#0x6b8]
1083f33c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
