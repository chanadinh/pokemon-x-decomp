108a9b28  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a9b2c  183093e5  ldr r3, [r3,#0x18]
108a9b30  000053e3  cmp r3, #0x0
108a9b34  2300000a  beq 0x108a9bc8
108a9b38  0060a0e1  cpy r6, r0
108a9b3c  0170a0e1  cpy r7, r1
108a9b40  0240a0e1  cpy r4, r2
108a9b44  0200a0e3  mov r0, #0x2
108a9b48  7307ffeb  bl 0x1086b91c
108a9b4c  040050e1  cmp r0, r4
108a9b50  1c00001a  bne 0x108a9bc8
108a9b54  0420a0e1  cpy r2, r4
108a9b58  1110a0e3  mov r1, #0x11
108a9b5c  0700a0e1  cpy r0, r7
108a9b60  0501ffeb  bl 0x10869f7c
108a9b64  0010a0e1  cpy r1, r0
108a9b68  0440c0e5  strb r4, [r0,#0x4]
108a9b6c  0700a0e1  cpy r0, r7
108a9b70  f4f7feeb  bl 0x10867b48
108a9b74  0420a0e1  cpy r2, r4
108a9b78  0310a0e3  mov r1, #0x3
108a9b7c  0700a0e1  cpy r0, r7
108a9b80  fd00ffeb  bl 0x10869f7c
108a9b84  0050a0e1  cpy r5, r0
108a9b88  fe2fa0e3  mov r2, #0x3f8
108a9b8c  0210a0e3  mov r1, #0x2
108a9b90  040080e2  add r0, r0, #0x4
108a9b94  ce8dfeeb  bl 0x1084d2d4
108a9b98  0410a0e1  cpy r1, r4
108a9b9c  040085e2  add r0, r5, #0x4
108a9ba0  bd8dfeeb  bl 0x1084d29c
108a9ba4  0600a0e1  cpy r0, r6
108a9ba8  b803d0e1  ldrh r0, [r0,#0x38]
108a9bac  0010a0e1  cpy r1, r0
108a9bb0  040085e2  add r0, r5, #0x4
108a9bb4  b88dfeeb  bl 0x1084d29c
108a9bb8  0510a0e1  cpy r1, r5
108a9bbc  0700a0e1  cpy r0, r7
108a9bc0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a9bc4  dff7feea  b 0x10867b48
108a9bc8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
