108a65c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a65cc  0050a0e1  cpy r5, r0
108a65d0  0240a0e1  cpy r4, r2
108a65d4  0300a0e3  mov r0, #0x3
108a65d8  cf14ffeb  bl 0x1086b91c
108a65dc  040050e1  cmp r0, r4
108a65e0  0900001a  bne 0x108a660c
108a65e4  1600a0e3  mov r0, #0x16
108a65e8  cb14ffeb  bl 0x1086b91c
108a65ec  060050e3  cmp r0, #0x6
108a65f0  0500001a  bne 0x108a660c
108a65f4  0500a0e1  cpy r0, r5
108a65f8  7999feeb  bl 0x1084cbe4
108a65fc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6600  0010a0e1  cpy r1, r0
108a6604  3200a0e3  mov r0, #0x32
108a6608  1d9afeea  b 0x1084ce84
108a660c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
