108a6ce8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6cec  0040a0e1  cpy r4, r0
108a6cf0  0250a0e1  cpy r5, r2
108a6cf4  0300a0e3  mov r0, #0x3
108a6cf8  0713ffeb  bl 0x1086b91c
108a6cfc  050050e1  cmp r0, r5
108a6d00  0c00001a  bne 0x108a6d38
108a6d04  3a00a0e3  mov r0, #0x3a
108a6d08  0313ffeb  bl 0x1086b91c
108a6d0c  030050e3  cmp r0, #0x3
108a6d10  0800001a  bne 0x108a6d38
108a6d14  0400a0e1  cpy r0, r4
108a6d18  0350a0e3  mov r5, #0x3
108a6d1c  b803d0e1  ldrh r0, [r0,#0x38]
108a6d20  0510a0e1  cpy r1, r5
108a6d24  9298feeb  bl 0x1084cf74
108a6d28  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6d2c  0010a0e1  cpy r1, r0
108a6d30  2500a0e3  mov r0, #0x25
108a6d34  e6fafeea  b 0x108658d4
108a6d38  7080bde8  ldmia sp!,{r4,r5,r6,pc}
