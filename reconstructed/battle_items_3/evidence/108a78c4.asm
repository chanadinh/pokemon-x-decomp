108a78c4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a78c8  0050a0e1  cpy r5, r0
108a78cc  0240a0e1  cpy r4, r2
108a78d0  0300a0e3  mov r0, #0x3
108a78d4  1010ffeb  bl 0x1086b91c
108a78d8  040050e1  cmp r0, r4
108a78dc  0900001a  bne 0x108a7908
108a78e0  1600a0e3  mov r0, #0x16
108a78e4  0c10ffeb  bl 0x1086b91c
108a78e8  070050e3  cmp r0, #0x7
108a78ec  0500001a  bne 0x108a7908
108a78f0  0500a0e1  cpy r0, r5
108a78f4  ba94feeb  bl 0x1084cbe4
108a78f8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a78fc  0010a0e1  cpy r1, r0
108a7900  3200a0e3  mov r0, #0x32
108a7904  5e95feea  b 0x1084ce84
108a7908  7080bde8  ldmia sp!,{r4,r5,r6,pc}
