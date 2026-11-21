108b7e50  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b7e54  0250a0e1  cpy r5, r2
108b7e58  0170a0e1  cpy r7, r1
108b7e5c  0360a0e1  cpy r6, r3
108b7e60  0300a0e3  mov r0, #0x3
108b7e64  accefeeb  bl 0x1086b91c
108b7e68  050050e1  cmp r0, r5
108b7e6c  1000001a  bne 0x108b7eb4
108b7e70  0520a0e1  cpy r2, r5
108b7e74  0310a0e3  mov r1, #0x3
108b7e78  0700a0e1  cpy r0, r7
108b7e7c  3ec8feeb  bl 0x10869f7c
108b7e80  0040a0e1  cpy r4, r0
108b7e84  2c209fe5  ldr r2, [0x108b7eb8]
108b7e88  0210a0e3  mov r1, #0x2
108b7e8c  040080e2  add r0, r0, #0x4
108b7e90  0f55feeb  bl 0x1084d2d4
108b7e94  0510a0e1  cpy r1, r5
108b7e98  040084e2  add r0, r4, #0x4
108b7e9c  fe54feeb  bl 0x1084d29c
108b7ea0  0410a0e1  cpy r1, r4
108b7ea4  0700a0e1  cpy r0, r7
108b7ea8  26bffeeb  bl 0x10867b48
108b7eac  0100a0e3  mov r0, #0x1
108b7eb0  180086e5  str r0, [r6,#0x18]
108b7eb4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
