108a61a8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a61ac  0160a0e1  cpy r6, r1
108a61b0  0070a0e1  cpy r7, r0
108a61b4  0250a0e1  cpy r5, r2
108a61b8  0200a0e3  mov r0, #0x2
108a61bc  d615ffeb  bl 0x1086b91c
108a61c0  050050e1  cmp r0, r5
108a61c4  1e00001a  bne 0x108a6244
108a61c8  0520a0e1  cpy r2, r5
108a61cc  0b10a0e3  mov r1, #0xb
108a61d0  0600a0e1  cpy r0, r6
108a61d4  680fffeb  bl 0x10869f7c
108a61d8  0040a0e1  cpy r4, r0
108a61dc  0400a0e3  mov r0, #0x4
108a61e0  b400c4e1  strh r0, [r4,#0x4]
108a61e4  05ebfeeb  bl 0x10860e00
108a61e8  080084e5  str r0, [r4,#0x8]
108a61ec  0010a0e3  mov r1, #0x0
108a61f0  0c10c4e5  strb r1, [r4,#0xc]
108a61f4  0700a0e1  cpy r0, r7
108a61f8  0f50c4e5  strb r5, [r4,#0xf]
108a61fc  b803d0e1  ldrh r0, [r0,#0x38]
108a6200  1c0084e5  str r0, [r4,#0x1c]
108a6204  3c209fe5  ldr r2, [0x108a6248]
108a6208  0210a0e3  mov r1, #0x2
108a620c  140084e2  add r0, r4, #0x14
108a6210  2f9cfeeb  bl 0x1084d2d4
108a6214  0510a0e1  cpy r1, r5
108a6218  140084e2  add r0, r4, #0x14
108a621c  1e9cfeeb  bl 0x1084d29c
108a6220  0700a0e1  cpy r0, r7
108a6224  b803d0e1  ldrh r0, [r0,#0x38]
108a6228  0010a0e1  cpy r1, r0
108a622c  140084e2  add r0, r4, #0x14
108a6230  199cfeeb  bl 0x1084d29c
108a6234  0410a0e1  cpy r1, r4
108a6238  0600a0e1  cpy r0, r6
108a623c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a6240  4006ffea  b 0x10867b48
108a6244  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
