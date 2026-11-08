108a9a4c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a9a50  0250a0e1  cpy r5, r2
108a9a54  0080a0e1  cpy r8, r0
108a9a58  0170a0e1  cpy r7, r1
108a9a5c  0360a0e1  cpy r6, r3
108a9a60  0200a0e3  mov r0, #0x2
108a9a64  ac07ffeb  bl 0x1086b91c
108a9a68  050050e1  cmp r0, r5
108a9a6c  1500001a  bne 0x108a9ac8
108a9a70  0520a0e1  cpy r2, r5
108a9a74  0310a0e3  mov r1, #0x3
108a9a78  0700a0e1  cpy r0, r7
108a9a7c  3e01ffeb  bl 0x10869f7c
108a9a80  0040a0e1  cpy r4, r0
108a9a84  40209fe5  ldr r2, [0x108a9acc]
108a9a88  0210a0e3  mov r1, #0x2
108a9a8c  040080e2  add r0, r0, #0x4
108a9a90  0f8efeeb  bl 0x1084d2d4
108a9a94  0510a0e1  cpy r1, r5
108a9a98  040084e2  add r0, r4, #0x4
108a9a9c  fe8dfeeb  bl 0x1084d29c
108a9aa0  0800a0e1  cpy r0, r8
108a9aa4  b803d0e1  ldrh r0, [r0,#0x38]
108a9aa8  0010a0e1  cpy r1, r0
108a9aac  040084e2  add r0, r4, #0x4
108a9ab0  f98dfeeb  bl 0x1084d29c
108a9ab4  0410a0e1  cpy r1, r4
108a9ab8  0700a0e1  cpy r0, r7
108a9abc  21f8feeb  bl 0x10867b48
108a9ac0  0000a0e3  mov r0, #0x0
108a9ac4  000086e5  str r0, [r6,#0x0]
108a9ac8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
