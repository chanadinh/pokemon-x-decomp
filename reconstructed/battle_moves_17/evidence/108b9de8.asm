108b9de8  10402de9  stmdb sp!,{r4,lr}
108b9dec  0240a0e1  cpy r4, r2
108b9df0  0300a0e3  mov r0, #0x3
108b9df4  c8c6feeb  bl 0x1086b91c
108b9df8  040050e1  cmp r0, r4
108b9dfc  0300001a  bne 0x108b9e10
108b9e00  1040bde8  ldmia sp!,{r4,lr}
108b9e04  0110a0e3  mov r1, #0x1
108b9e08  5300a0e3  mov r0, #0x53
108b9e0c  b0aefeea  b 0x108658d4
108b9e10  1080bde8  ldmia sp!,{r4,pc}
