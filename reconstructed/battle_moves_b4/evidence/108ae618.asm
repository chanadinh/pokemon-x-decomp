108ae618  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108ae61c  0240a0e1  cpy r4, r2
108ae620  0170a0e1  cpy r7, r1
108ae624  0300a0e3  mov r0, #0x3
108ae628  bbf4feeb  bl 0x1086b91c
108ae62c  040050e1  cmp r0, r4
108ae630  3500001a  bne 0x108ae70c
108ae634  0600a0e3  mov r0, #0x6
108ae638  b7f4feeb  bl 0x1086b91c
108ae63c  ff5000e2  and r5, r0, #0xff
108ae640  0510a0e1  cpy r1, r5
108ae644  0700a0e1  cpy r0, r7
108ae648  9497feeb  bl 0x108544a0
108ae64c  0090a0e1  cpy r9, r0
108ae650  0877feeb  bl 0x1084c278
108ae654  0080a0e1  cpy r8, r0
108ae658  1710a0e3  mov r1, #0x17
108ae65c  0900a0e1  cpy r0, r9
108ae660  08a1feeb  bl 0x10856a88
108ae664  000050e3  cmp r0, #0x0
108ae668  2700001a  bne 0x108ae70c
108ae66c  0800a0e1  cpy r0, r8
108ae670  63d8ffeb  bl 0x108a4804
108ae674  000050e3  cmp r0, #0x0
108ae678  2300001a  bne 0x108ae70c
108ae67c  0810a0e1  cpy r1, r8
108ae680  0900a0e1  cpy r0, r9
108ae684  0beefeeb  bl 0x10869eb8
108ae688  000050e3  cmp r0, #0x0
108ae68c  1e00000a  beq 0x108ae70c
108ae690  0360a0e3  mov r6, #0x3
108ae694  0110a0e3  mov r1, #0x1
108ae698  0900a0e1  cpy r0, r9
108ae69c  5d78feeb  bl 0x1084c818
108ae6a0  000050e3  cmp r0, #0x0
108ae6a4  0460a013  movne r6, #0x4
108ae6a8  0420a0e1  cpy r2, r4
108ae6ac  0b10a0e3  mov r1, #0xb
108ae6b0  0700a0e1  cpy r0, r7
108ae6b4  30eefeeb  bl 0x10869f7c
108ae6b8  0040a0e1  cpy r4, r0
108ae6bc  1700a0e3  mov r0, #0x17
108ae6c0  b400c4e1  strh r0, [r4,#0x4]
108ae6c4  0810a0e1  cpy r1, r8
108ae6c8  0600a0e1  cpy r0, r6
108ae6cc  f675feeb  bl 0x1084beac
108ae6d0  080084e5  str r0, [r4,#0x8]
108ae6d4  0010a0e3  mov r1, #0x0
108ae6d8  0f50c4e5  strb r5, [r4,#0xf]
108ae6dc  0c10c4e5  strb r1, [r4,#0xc]
108ae6e0  28209fe5  ldr r2, [0x108ae710]
108ae6e4  0210a0e3  mov r1, #0x2
108ae6e8  140084e2  add r0, r4, #0x14
108ae6ec  f87afeeb  bl 0x1084d2d4
108ae6f0  0510a0e1  cpy r1, r5
108ae6f4  140084e2  add r0, r4, #0x14
108ae6f8  e77afeeb  bl 0x1084d29c
108ae6fc  0410a0e1  cpy r1, r4
108ae700  0700a0e1  cpy r0, r7
108ae704  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108ae708  0ee5feea  b 0x10867b48
108ae70c  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
