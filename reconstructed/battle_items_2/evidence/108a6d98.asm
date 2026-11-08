108a6d98  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6d9c  0160a0e1  cpy r6, r1
108a6da0  0240a0e1  cpy r4, r2
108a6da4  0350a0e3  mov r5, #0x3
108a6da8  0200a0e3  mov r0, #0x2
108a6dac  da12ffeb  bl 0x1086b91c
108a6db0  040050e1  cmp r0, r4
108a6db4  0c00001a  bne 0x108a6dec
108a6db8  0420a0e1  cpy r2, r4
108a6dbc  0d10a0e3  mov r1, #0xd
108a6dc0  0600a0e1  cpy r0, r6
108a6dc4  6c0cffeb  bl 0x10869f7c
108a6dc8  0120a0e3  mov r2, #0x1
108a6dcc  0450c0e5  strb r5, [r0,#0x4]
108a6dd0  1720c0e5  strb r2, [r0,#0x17]
108a6dd4  0c40c0e5  strb r4, [r0,#0xc]
108a6dd8  0010a0e1  cpy r1, r0
108a6ddc  1620c0e5  strb r2, [r0,#0x16]
108a6de0  0600a0e1  cpy r0, r6
108a6de4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6de8  5603ffea  b 0x10867b48
108a6dec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
