108b8f8c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8f90  0150a0e1  cpy r5, r1
108b8f94  0240a0e1  cpy r4, r2
108b8f98  0300a0e3  mov r0, #0x3
108b8f9c  5ecafeeb  bl 0x1086b91c
108b8fa0  040050e1  cmp r0, r4
108b8fa4  0900001a  bne 0x108b8fd0
108b8fa8  0410a0e1  cpy r1, r4
108b8fac  0500a0e1  cpy r0, r5
108b8fb0  3a6dfeeb  bl 0x108544a0
108b8fb4  b201d0e1  ldrh r0, [r0,#0x12]
108b8fb8  0710a0e3  mov r1, #0x7
108b8fbc  ec4ffeeb  bl 0x1084cf74
108b8fc0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8fc4  0010a0e1  cpy r1, r0
108b8fc8  3100a0e3  mov r0, #0x31
108b8fcc  40b2feea  b 0x108658d4
108b8fd0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
