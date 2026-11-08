108a8f24  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8f28  0160a0e1  cpy r6, r1
108a8f2c  0250a0e1  cpy r5, r2
108a8f30  0200a0e3  mov r0, #0x2
108a8f34  780affeb  bl 0x1086b91c
108a8f38  050050e1  cmp r0, r5
108a8f3c  1400001a  bne 0x108a8f94
108a8f40  0210a0e3  mov r1, #0x2
108a8f44  0600a0e1  cpy r0, r6
108a8f48  098dfeeb  bl 0x1084c374
108a8f4c  000050e3  cmp r0, #0x0
108a8f50  0f00001a  bne 0x108a8f94
108a8f54  0520a0e1  cpy r2, r5
108a8f58  0310a0e3  mov r1, #0x3
108a8f5c  0600a0e1  cpy r0, r6
108a8f60  0504ffeb  bl 0x10869f7c
108a8f64  0040a0e1  cpy r4, r0
108a8f68  662fa0e3  mov r2, #0x198
108a8f6c  0210a0e3  mov r1, #0x2
108a8f70  040080e2  add r0, r0, #0x4
108a8f74  d690feeb  bl 0x1084d2d4
108a8f78  0510a0e1  cpy r1, r5
108a8f7c  040084e2  add r0, r4, #0x4
108a8f80  c590feeb  bl 0x1084d29c
108a8f84  0410a0e1  cpy r1, r4
108a8f88  0600a0e1  cpy r0, r6
108a8f8c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8f90  ecfafeea  b 0x10867b48
108a8f94  7080bde8  ldmia sp!,{r4,r5,r6,pc}
