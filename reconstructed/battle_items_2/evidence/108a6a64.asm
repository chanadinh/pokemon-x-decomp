108a6a64  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6a68  0160a0e1  cpy r6, r1
108a6a6c  0240a0e1  cpy r4, r2
108a6a70  0550a0e3  mov r5, #0x5
108a6a74  0200a0e3  mov r0, #0x2
108a6a78  a713ffeb  bl 0x1086b91c
108a6a7c  040050e1  cmp r0, r4
108a6a80  0c00001a  bne 0x108a6ab8
108a6a84  0420a0e1  cpy r2, r4
108a6a88  0d10a0e3  mov r1, #0xd
108a6a8c  0600a0e1  cpy r0, r6
108a6a90  390dffeb  bl 0x10869f7c
108a6a94  0120a0e3  mov r2, #0x1
108a6a98  0450c0e5  strb r5, [r0,#0x4]
108a6a9c  1720c0e5  strb r2, [r0,#0x17]
108a6aa0  0c40c0e5  strb r4, [r0,#0xc]
108a6aa4  0010a0e1  cpy r1, r0
108a6aa8  1620c0e5  strb r2, [r0,#0x16]
108a6aac  0600a0e1  cpy r0, r6
108a6ab0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6ab4  2304ffea  b 0x10867b48
108a6ab8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
