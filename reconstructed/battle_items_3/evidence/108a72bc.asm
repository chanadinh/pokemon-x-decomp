108a72bc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a72c0  0050a0e1  cpy r5, r0
108a72c4  0240a0e1  cpy r4, r2
108a72c8  0300a0e3  mov r0, #0x3
108a72cc  9211ffeb  bl 0x1086b91c
108a72d0  040050e1  cmp r0, r4
108a72d4  0900001a  bne 0x108a7300
108a72d8  1600a0e3  mov r0, #0x16
108a72dc  8e11ffeb  bl 0x1086b91c
108a72e0  0b0050e3  cmp r0, #0xb
108a72e4  0500001a  bne 0x108a7300
108a72e8  0500a0e1  cpy r0, r5
108a72ec  3c96feeb  bl 0x1084cbe4
108a72f0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a72f4  0010a0e1  cpy r1, r0
108a72f8  3200a0e3  mov r0, #0x32
108a72fc  e096feea  b 0x1084ce84
108a7300  7080bde8  ldmia sp!,{r4,r5,r6,pc}
