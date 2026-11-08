108a92b0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a92b4  0050a0e1  cpy r5, r0
108a92b8  0240a0e1  cpy r4, r2
108a92bc  0300a0e3  mov r0, #0x3
108a92c0  9509ffeb  bl 0x1086b91c
108a92c4  040050e1  cmp r0, r4
108a92c8  0900001a  bne 0x108a92f4
108a92cc  1600a0e3  mov r0, #0x16
108a92d0  9109ffeb  bl 0x1086b91c
108a92d4  020050e3  cmp r0, #0x2
108a92d8  0500001a  bne 0x108a92f4
108a92dc  0500a0e1  cpy r0, r5
108a92e0  3f8efeeb  bl 0x1084cbe4
108a92e4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a92e8  0010a0e1  cpy r1, r0
108a92ec  3200a0e3  mov r0, #0x32
108a92f0  e38efeea  b 0x1084ce84
108a92f4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
