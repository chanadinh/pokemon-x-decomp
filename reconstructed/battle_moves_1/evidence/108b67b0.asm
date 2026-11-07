108b67b0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b67b4  0240a0e1  cpy r4, r2
108b67b8  0350a0e1  cpy r5, r3
108b67bc  0300a0e3  mov r0, #0x3
108b67c0  55d4feeb  bl 0x1086b91c
108b67c4  040050e1  cmp r0, r4
108b67c8  0600001a  bne 0x108b67e8
108b67cc  000095e5  ldr r0, [r5,#0x0]
108b67d0  000050e3  cmp r0, #0x0
108b67d4  0300000a  beq 0x108b67e8
108b67d8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b67dc  9610a0e3  mov r1, #0x96
108b67e0  3100a0e3  mov r0, #0x31
108b67e4  3abcfeea  b 0x108658d4
108b67e8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
