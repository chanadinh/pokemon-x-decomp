108b836c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b8370  0250a0e1  cpy r5, r2
108b8374  0170a0e1  cpy r7, r1
108b8378  0360a0e1  cpy r6, r3
108b837c  0200a0e3  mov r0, #0x2
108b8380  65cdfeeb  bl 0x1086b91c
108b8384  050050e1  cmp r0, r5
108b8388  1b00001a  bne 0x108b83fc
108b838c  6400a0e3  mov r0, #0x64
108b8390  0951feeb  bl 0x1084c7bc
108b8394  ff1000e2  and r1, r0, #0xff
108b8398  60009fe5  ldr r0, [0x108b8400]
108b839c  0040a0e3  mov r4, #0x0
108b83a0  8420d0e7  ldrb r2, [r0,r4,lsl #0x1]
108b83a4  010052e1  cmp r2, r1
108b83a8  0300008a  bhi 0x108b83bc
108b83ac  012084e2  add r2, r4, #0x1
108b83b0  ff4002e2  and r4, r2, #0xff
108b83b4  070054e3  cmp r4, #0x7
108b83b8  f8ffff3a  bcc 0x108b83a0
108b83bc  840080e0  add r0, r0, r4, lsl #0x1
108b83c0  0520a0e1  cpy r2, r5
108b83c4  0100d0e5  ldrb r0, [r0,#0x1]
108b83c8  0310a0e3  mov r1, #0x3
108b83cc  000086e5  str r0, [r6,#0x0]
108b83d0  0700a0e1  cpy r0, r7
108b83d4  e8c6feeb  bl 0x10869f7c
108b83d8  0050a0e1  cpy r5, r0
108b83dc  6c2084e2  add r2, r4, #0x6c
108b83e0  0110a0e3  mov r1, #0x1
108b83e4  040080e2  add r0, r0, #0x4
108b83e8  b953feeb  bl 0x1084d2d4
108b83ec  0510a0e1  cpy r1, r5
108b83f0  0700a0e1  cpy r0, r7
108b83f4  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b83f8  d2bdfeea  b 0x10867b48
108b83fc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
