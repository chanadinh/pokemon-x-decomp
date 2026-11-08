108a6c74  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6c78  0040a0e1  cpy r4, r0
108a6c7c  0250a0e1  cpy r5, r2
108a6c80  0300a0e3  mov r0, #0x3
108a6c84  2413ffeb  bl 0x1086b91c
108a6c88  050050e1  cmp r0, r5
108a6c8c  0c00001a  bne 0x108a6cc4
108a6c90  3a00a0e3  mov r0, #0x3a
108a6c94  2013ffeb  bl 0x1086b91c
108a6c98  020050e3  cmp r0, #0x2
108a6c9c  0800001a  bne 0x108a6cc4
108a6ca0  0400a0e1  cpy r0, r4
108a6ca4  0350a0e3  mov r5, #0x3
108a6ca8  b803d0e1  ldrh r0, [r0,#0x38]
108a6cac  0510a0e1  cpy r1, r5
108a6cb0  af98feeb  bl 0x1084cf74
108a6cb4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6cb8  0010a0e1  cpy r1, r0
108a6cbc  2500a0e3  mov r0, #0x25
108a6cc0  03fbfeea  b 0x108658d4
108a6cc4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
