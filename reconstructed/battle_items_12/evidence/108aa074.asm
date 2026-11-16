108aa074  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aa078  183093e5  ldr r3, [r3,#0x18]
108aa07c  000053e3  cmp r3, #0x0
108aa080  2400000a  beq 0x108aa118
108aa084  0060a0e1  cpy r6, r0
108aa088  0170a0e1  cpy r7, r1
108aa08c  0250a0e1  cpy r5, r2
108aa090  0200a0e3  mov r0, #0x2
108aa094  2006ffeb  bl 0x1086b91c
108aa098  050050e1  cmp r0, r5
108aa09c  1d00001a  bne 0x108aa118
108aa0a0  0510a0e1  cpy r1, r5
108aa0a4  0700a0e1  cpy r0, r7
108aa0a8  fca8feeb  bl 0x108544a0
108aa0ac  0520a0e1  cpy r2, r5
108aa0b0  0b10a0e3  mov r1, #0xb
108aa0b4  0700a0e1  cpy r0, r7
108aa0b8  affffeeb  bl 0x10869f7c
108aa0bc  0040a0e1  cpy r4, r0
108aa0c0  2200a0e3  mov r0, #0x22
108aa0c4  0f50c4e5  strb r5, [r4,#0xf]
108aa0c8  b400c4e1  strh r0, [r4,#0x4]
108aa0cc  7800a0e3  mov r0, #0x78
108aa0d0  b688feeb  bl 0x1084c3b0
108aa0d4  080084e5  str r0, [r4,#0x8]
108aa0d8  3c209fe5  ldr r2, [0x108aa11c]
108aa0dc  0210a0e3  mov r1, #0x2
108aa0e0  140084e2  add r0, r4, #0x14
108aa0e4  7a8cfeeb  bl 0x1084d2d4
108aa0e8  0510a0e1  cpy r1, r5
108aa0ec  140084e2  add r0, r4, #0x14
108aa0f0  698cfeeb  bl 0x1084d29c
108aa0f4  0600a0e1  cpy r0, r6
108aa0f8  b803d0e1  ldrh r0, [r0,#0x38]
108aa0fc  0010a0e1  cpy r1, r0
108aa100  140084e2  add r0, r4, #0x14
108aa104  648cfeeb  bl 0x1084d29c
108aa108  0410a0e1  cpy r1, r4
108aa10c  0700a0e1  cpy r0, r7
108aa110  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aa114  8bf6feea  b 0x10867b48
108aa118  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
