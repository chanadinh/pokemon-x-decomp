108a7788  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a778c  0150a0e1  cpy r5, r1
108a7790  0240a0e1  cpy r4, r2
108a7794  0200a0e3  mov r0, #0x2
108a7798  5f10ffeb  bl 0x1086b91c
108a779c  040050e1  cmp r0, r4
108a77a0  0c00001a  bne 0x108a77d8
108a77a4  0420a0e1  cpy r2, r4
108a77a8  0d10a0e3  mov r1, #0xd
108a77ac  0500a0e1  cpy r0, r5
108a77b0  f109ffeb  bl 0x10869f7c
108a77b4  0120a0e3  mov r2, #0x1
108a77b8  0c40c0e5  strb r4, [r0,#0xc]
108a77bc  1620c0e5  strb r2, [r0,#0x16]
108a77c0  0420c0e5  strb r2, [r0,#0x4]
108a77c4  0010a0e1  cpy r1, r0
108a77c8  1720c0e5  strb r2, [r0,#0x17]
108a77cc  0500a0e1  cpy r0, r5
108a77d0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a77d4  db00ffea  b 0x10867b48
108a77d8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
