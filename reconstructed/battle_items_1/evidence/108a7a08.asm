108a7a08  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a7a0c  0060a0e1  cpy r6, r0
108a7a10  0180a0e1  cpy r8, r1
108a7a14  0250a0e1  cpy r5, r2
108a7a18  0200a0e3  mov r0, #0x2
108a7a1c  be0fffeb  bl 0x1086b91c
108a7a20  050050e1  cmp r0, r5
108a7a24  1b00001a  bne 0x108a7a98
108a7a28  0520a0e1  cpy r2, r5
108a7a2c  0410a0e3  mov r1, #0x4
108a7a30  0800a0e1  cpy r0, r8
108a7a34  5009ffeb  bl 0x10869f7c
108a7a38  0040a0e1  cpy r4, r0
108a7a3c  0650c0e5  strb r5, [r0,#0x6]
108a7a40  0600a0e1  cpy r0, r6
108a7a44  0370a0e3  mov r7, #0x3
108a7a48  b803d0e1  ldrh r0, [r0,#0x38]
108a7a4c  0710a0e1  cpy r1, r7
108a7a50  4795feeb  bl 0x1084cf74
108a7a54  b400c4e1  strh r0, [r4,#0x4]
108a7a58  3c209fe5  ldr r2, [0x108a7a9c]
108a7a5c  0210a0e3  mov r1, #0x2
108a7a60  080084e2  add r0, r4, #0x8
108a7a64  1a96feeb  bl 0x1084d2d4
108a7a68  0510a0e1  cpy r1, r5
108a7a6c  080084e2  add r0, r4, #0x8
108a7a70  0996feeb  bl 0x1084d29c
108a7a74  0600a0e1  cpy r0, r6
108a7a78  b803d0e1  ldrh r0, [r0,#0x38]
108a7a7c  0010a0e1  cpy r1, r0
108a7a80  080084e2  add r0, r4, #0x8
108a7a84  0496feeb  bl 0x1084d29c
108a7a88  0410a0e1  cpy r1, r4
108a7a8c  0800a0e1  cpy r0, r8
108a7a90  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a7a94  2b00ffea  b 0x10867b48
108a7a98  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
