0011e264  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
0011e268  0040a0e1  cpy r4, r0
0011e26c  20008de2  add r0, sp, #0x20
0011e270  0190a0e1  cpy r9, r1
0011e274  400190e8  ldmia r0,{r6,r8}
0011e278  0000a0e3  mov r0, #0x0
0011e27c  80109fe5  ldr r1, [0x11e304]
0011e280  0350a0e1  cpy r5, r3
0011e284  080084e5  str r0, [r4,#0x8]
0011e288  220084e8  stmia r4,{r1,r5}
0011e28c  02a0a0e1  cpy r10, r2
0011e290  0c60c4e5  strb r6, [r4,#0xc]
0011e294  100084e5  str r0, [r4,#0x10]
0011e298  140084e5  str r0, [r4,#0x14]
0011e29c  180084e5  str r0, [r4,#0x18]
0011e2a0  1c0084e5  str r0, [r4,#0x1c]
0011e2a4  000093e5  ldr r0, [r3,#0x0]
0011e2a8  7870a0e3  mov r7, #0x78
0011e2ac  1c1090e5  ldr r1, [r0,#0x1c]
0011e2b0  0500a0e1  cpy r0, r5
0011e2b4  31ff2fe1  blx r1
0011e2b8  0020a0e1  cpy r2, r0
0011e2bc  0130a0e3  mov r3, #0x1
0011e2c0  0710a0e1  cpy r1, r7
0011e2c4  0500a0e1  cpy r0, r5
0011e2c8  7e3300eb  bl 0x0012b0c8
0011e2cc  000050e3  cmp r0, #0x0
0011e2d0  0300000a  beq 0x0011e2e4
0011e2d4  0830a0e1  cpy r3, r8
0011e2d8  0920a0e1  cpy r2, r9
0011e2dc  0510a0e1  cpy r1, r5
0011e2e0  e82700eb  bl 0x00128288
0011e2e4  080084e5  str r0, [r4,#0x8]
0011e2e8  0630a0e1  cpy r3, r6
0011e2ec  0520a0e1  cpy r2, r5
0011e2f0  0a10a0e1  cpy r1, r10
0011e2f4  0400a0e1  cpy r0, r4
0011e2f8  9d3200eb  bl 0x0012ad74
0011e2fc  0400a0e1  cpy r0, r4
0011e300  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
