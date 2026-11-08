108a9fc0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a9fc4  0150a0e1  cpy r5, r1
108a9fc8  0240a0e1  cpy r4, r2
108a9fcc  0200a0e3  mov r0, #0x2
108a9fd0  5106ffeb  bl 0x1086b91c
108a9fd4  040050e1  cmp r0, r4
108a9fd8  1900001a  bne 0x108aa044
108a9fdc  0420a0e1  cpy r2, r4
108a9fe0  0d10a0e3  mov r1, #0xd
108a9fe4  0500a0e1  cpy r0, r5
108a9fe8  e3fffeeb  bl 0x10869f7c
108a9fec  0160a0e3  mov r6, #0x1
108a9ff0  0270a0e3  mov r7, #0x2
108a9ff4  0460c0e5  strb r6, [r0,#0x4]
108a9ff8  1770c0e5  strb r7, [r0,#0x17]
108a9ffc  1660c0e5  strb r6, [r0,#0x16]
108aa000  0010a0e1  cpy r1, r0
108aa004  0c40c0e5  strb r4, [r0,#0xc]
108aa008  0500a0e1  cpy r0, r5
108aa00c  cdf6feeb  bl 0x10867b48
108aa010  0420a0e1  cpy r2, r4
108aa014  0d10a0e3  mov r1, #0xd
108aa018  0500a0e1  cpy r0, r5
108aa01c  d6fffeeb  bl 0x10869f7c
108aa020  0010a0e1  cpy r1, r0
108aa024  0300a0e3  mov r0, #0x3
108aa028  1770c1e5  strb r7, [r1,#0x17]
108aa02c  1660c1e5  strb r6, [r1,#0x16]
108aa030  0400c1e5  strb r0, [r1,#0x4]
108aa034  0500a0e1  cpy r0, r5
108aa038  0c40c1e5  strb r4, [r1,#0xc]
108aa03c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aa040  c0f6feea  b 0x10867b48
108aa044  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
