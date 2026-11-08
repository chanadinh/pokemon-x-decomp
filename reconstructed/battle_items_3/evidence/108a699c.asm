108a699c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a69a0  0050a0e1  cpy r5, r0
108a69a4  0240a0e1  cpy r4, r2
108a69a8  0300a0e3  mov r0, #0x3
108a69ac  da13ffeb  bl 0x1086b91c
108a69b0  040050e1  cmp r0, r4
108a69b4  0900001a  bne 0x108a69e0
108a69b8  1600a0e3  mov r0, #0x16
108a69bc  d613ffeb  bl 0x1086b91c
108a69c0  0d0050e3  cmp r0, #0xd
108a69c4  0500001a  bne 0x108a69e0
108a69c8  0500a0e1  cpy r0, r5
108a69cc  8498feeb  bl 0x1084cbe4
108a69d0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a69d4  0010a0e1  cpy r1, r0
108a69d8  3200a0e3  mov r0, #0x32
108a69dc  2899feea  b 0x1084ce84
108a69e0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
