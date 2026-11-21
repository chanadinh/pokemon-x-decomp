108b3b18  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b3b1c  0250a0e1  cpy r5, r2
108b3b20  0170a0e1  cpy r7, r1
108b3b24  0300a0e3  mov r0, #0x3
108b3b28  7bdffeeb  bl 0x1086b91c
108b3b2c  050050e1  cmp r0, r5
108b3b30  2a00001a  bne 0x108b3be0
108b3b34  0600a0e3  mov r0, #0x6
108b3b38  77dffeeb  bl 0x1086b91c
108b3b3c  ff6000e2  and r6, r0, #0xff
108b3b40  0610a0e1  cpy r1, r6
108b3b44  0700a0e1  cpy r0, r7
108b3b48  5482feeb  bl 0x108544a0
108b3b4c  0080a0e1  cpy r8, r0
108b3b50  0520a0e1  cpy r2, r5
108b3b54  1310a0e3  mov r1, #0x13
108b3b58  0700a0e1  cpy r0, r7
108b3b5c  06d9feeb  bl 0x10869f7c
108b3b60  0040a0e1  cpy r4, r0
108b3b64  0650c0e5  strb r5, [r0,#0x6]
108b3b68  0800a0e1  cpy r0, r8
108b3b6c  88f3feeb  bl 0x10870994
108b3b70  b400c4e1  strh r0, [r4,#0x4]
108b3b74  0110a0e3  mov r1, #0x1
108b3b78  0710c4e5  strb r1, [r4,#0x7]
108b3b7c  0410a0e1  cpy r1, r4
108b3b80  0700a0e1  cpy r0, r7
108b3b84  efcffeeb  bl 0x10867b48
108b3b88  0520a0e1  cpy r2, r5
108b3b8c  0310a0e3  mov r1, #0x3
108b3b90  0700a0e1  cpy r0, r7
108b3b94  f8d8feeb  bl 0x10869f7c
108b3b98  0040a0e1  cpy r4, r0
108b3b9c  000090e5  ldr r0, [r0,#0x0]
108b3ba0  3c209fe5  ldr r2, [0x108b3be4]
108b3ba4  0210a0e3  mov r1, #0x2
108b3ba8  010480e3  orr r0, r0, #0x1000000
108b3bac  000084e5  str r0, [r4,#0x0]
108b3bb0  040084e2  add r0, r4, #0x4
108b3bb4  c665feeb  bl 0x1084d2d4
108b3bb8  0510a0e1  cpy r1, r5
108b3bbc  040084e2  add r0, r4, #0x4
108b3bc0  b565feeb  bl 0x1084d29c
108b3bc4  0610a0e1  cpy r1, r6
108b3bc8  040084e2  add r0, r4, #0x4
108b3bcc  b265feeb  bl 0x1084d29c
108b3bd0  0410a0e1  cpy r1, r4
108b3bd4  0700a0e1  cpy r0, r7
108b3bd8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b3bdc  d9cffeea  b 0x10867b48
108b3be0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
