108b9c70  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9c74  0150a0e1  cpy r5, r1
108b9c78  0240a0e1  cpy r4, r2
108b9c7c  0300a0e3  mov r0, #0x3
108b9c80  25c7feeb  bl 0x1086b91c
108b9c84  040050e1  cmp r0, r4
108b9c88  0700001a  bne 0x108b9cac
108b9c8c  0500a0e1  cpy r0, r5
108b9c90  7048feeb  bl 0x1084be58
108b9c94  010050e3  cmp r0, #0x1
108b9c98  0300001a  bne 0x108b9cac
108b9c9c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9ca0  0110a0e3  mov r1, #0x1
108b9ca4  5300a0e3  mov r0, #0x53
108b9ca8  09affeea  b 0x108658d4
108b9cac  7080bde8  ldmia sp!,{r4,r5,r6,pc}
