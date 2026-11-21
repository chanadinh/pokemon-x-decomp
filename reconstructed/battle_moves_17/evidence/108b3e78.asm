108b3e78  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108b3e7c  0150a0e1  cpy r5, r1
108b3e80  0260a0e1  cpy r6, r2
108b3e84  0d20a0e1  cpy r2, sp
108b3e88  0610a0e1  cpy r1, r6
108b3e8c  0500a0e1  cpy r0, r5
108b3e90  3b5efeeb  bl 0x1084b784
108b3e94  000050e3  cmp r0, #0x0
108b3e98  1700000a  beq 0x108b3efc
108b3e9c  0600a0e3  mov r0, #0x6
108b3ea0  9ddefeeb  bl 0x1086b91c
108b3ea4  ff4000e2  and r4, r0, #0xff
108b3ea8  0610a0e1  cpy r1, r6
108b3eac  0400a0e1  cpy r0, r4
108b3eb0  bc61feeb  bl 0x1084c5a8
108b3eb4  000050e3  cmp r0, #0x0
108b3eb8  0f00001a  bne 0x108b3efc
108b3ebc  0410a0e1  cpy r1, r4
108b3ec0  0500a0e1  cpy r0, r5
108b3ec4  a3fcfeeb  bl 0x10873158
108b3ec8  0610a0e1  cpy r1, r6
108b3ecc  0500a0e1  cpy r0, r5
108b3ed0  a0fcfeeb  bl 0x10873158
108b3ed4  0030a0e3  mov r3, #0x0
108b3ed8  0420a0e1  cpy r2, r4
108b3edc  0610a0e1  cpy r1, r6
108b3ee0  0500a0e1  cpy r0, r5
108b3ee4  e45dfeeb  bl 0x1084b67c
108b3ee8  000050e3  cmp r0, #0x0
108b3eec  0200001a  bne 0x108b3efc
108b3ef0  0610a0e1  cpy r1, r6
108b3ef4  0500a0e1  cpy r0, r5
108b3ef8  12a6fdeb  bl 0x1081d748
108b3efc  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
