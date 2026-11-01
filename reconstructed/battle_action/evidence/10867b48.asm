10867b48  70402de9  stmdb sp!,{r4,r5,r6,lr}
10867b4c  0050a0e1  cpy r5, r0
10867b50  0140a0e1  cpy r4, r1
10867b54  a9c6feeb  bl 0x10819600
10867b58  015985e2  add r5, r5, #0x4000
10867b5c  df5f85e2  add r5, r5, #0x37c
10867b60  000094e5  ldr r0, [r4,#0x0]
10867b64  002095e5  ldr r2, [r5,#0x0]
10867b68  8014a0e1  mov r1, r0, lsl #0x9
10867b6c  8236a0e1  mov r3, r2, lsl #0xd
10867b70  21cba0e1  mov r12, r1, lsr #0x16
10867b74  a30b5ce1  cmp r12, r3, lsr #0x17
10867b78  0a00008a  bhi 0x10867ba8
10867b7c  05c044e0  sub r12, r4, r5
10867b80  04c04ce2  sub r12, r12, #0x4
10867b84  211b8ce0  add r1, r12, r1, lsr #0x16
10867b88  a30b51e1  cmp r1, r3, lsr #0x17
10867b8c  0500001a  bne 0x10867ba8
10867b90  a006a0e1  mov r0, r0, lsr #0xd
10867b94  e219a0e1  mov r1, r2, ror #0x13
10867b98  800b41e0  sub r0, r1, r0, lsl #0x17
10867b9c  e006a0e1  mov r0, r0, ror #0xd
10867ba0  000085e5  str r0, [r5,#0x0]
10867ba4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
10867ba8  0020a0e3  mov r2, #0x0
10867bac  7040bde8  ldmia sp!,{r4,r5,r6,lr}
10867bb0  0210a0e1  cpy r1, r2
10867bb4  0200a0e1  cpy r0, r2
10867bb8  f561feea  b 0x10800394
