108a5de0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a5de4  0050a0e1  cpy r5, r0
108a5de8  0240a0e1  cpy r4, r2
108a5dec  0300a0e3  mov r0, #0x3
108a5df0  c916ffeb  bl 0x1086b91c
108a5df4  040050e1  cmp r0, r4
108a5df8  0900001a  bne 0x108a5e24
108a5dfc  1600a0e3  mov r0, #0x16
108a5e00  c516ffeb  bl 0x1086b91c
108a5e04  010050e3  cmp r0, #0x1
108a5e08  0500001a  bne 0x108a5e24
108a5e0c  0500a0e1  cpy r0, r5
108a5e10  739bfeeb  bl 0x1084cbe4
108a5e14  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a5e18  0010a0e1  cpy r1, r0
108a5e1c  3200a0e3  mov r0, #0x32
108a5e20  179cfeea  b 0x1084ce84
108a5e24  7080bde8  ldmia sp!,{r4,r5,r6,pc}
