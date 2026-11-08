108a5d60  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a5d64  0040a0e1  cpy r4, r0
108a5d68  0250a0e1  cpy r5, r2
108a5d6c  0300a0e3  mov r0, #0x3
108a5d70  e916ffeb  bl 0x1086b91c
108a5d74  050050e1  cmp r0, r5
108a5d78  0c00001a  bne 0x108a5db0
108a5d7c  3a00a0e3  mov r0, #0x3a
108a5d80  e516ffeb  bl 0x1086b91c
108a5d84  010050e3  cmp r0, #0x1
108a5d88  0800001a  bne 0x108a5db0
108a5d8c  0400a0e1  cpy r0, r4
108a5d90  0350a0e3  mov r5, #0x3
108a5d94  b803d0e1  ldrh r0, [r0,#0x38]
108a5d98  0510a0e1  cpy r1, r5
108a5d9c  749cfeeb  bl 0x1084cf74
108a5da0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a5da4  0010a0e1  cpy r1, r0
108a5da8  2500a0e3  mov r0, #0x25
108a5dac  c8fefeea  b 0x108658d4
108a5db0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
