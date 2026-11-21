108b987c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b9880  0250a0e1  cpy r5, r2
108b9884  0170a0e1  cpy r7, r1
108b9888  0360a0e1  cpy r6, r3
108b988c  0300a0e3  mov r0, #0x3
108b9890  21c8feeb  bl 0x1086b91c
108b9894  050050e1  cmp r0, r5
108b9898  1000001a  bne 0x108b98e0
108b989c  0520a0e1  cpy r2, r5
108b98a0  0310a0e3  mov r1, #0x3
108b98a4  0700a0e1  cpy r0, r7
108b98a8  b3c1feeb  bl 0x10869f7c
108b98ac  0040a0e1  cpy r4, r0
108b98b0  2c209fe5  ldr r2, [0x108b98e4]
108b98b4  0210a0e3  mov r1, #0x2
108b98b8  040080e2  add r0, r0, #0x4
108b98bc  844efeeb  bl 0x1084d2d4
108b98c0  0510a0e1  cpy r1, r5
108b98c4  040084e2  add r0, r4, #0x4
108b98c8  734efeeb  bl 0x1084d29c
108b98cc  0410a0e1  cpy r1, r4
108b98d0  0700a0e1  cpy r0, r7
108b98d4  9bb8feeb  bl 0x10867b48
108b98d8  0100a0e3  mov r0, #0x1
108b98dc  180086e5  str r0, [r6,#0x18]
108b98e0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
