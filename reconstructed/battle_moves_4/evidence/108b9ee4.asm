108b9ee4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9ee8  0050a0e1  cpy r5, r0
108b9eec  0240a0e1  cpy r4, r2
108b9ef0  0200a0e3  mov r0, #0x2
108b9ef4  88c6feeb  bl 0x1086b91c
108b9ef8  040050e1  cmp r0, r4
108b9efc  0200001a  bne 0x108b9f0c
108b9f00  0500a0e1  cpy r0, r5
108b9f04  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9f08  af4cfeea  b 0x1084d1cc
108b9f0c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
