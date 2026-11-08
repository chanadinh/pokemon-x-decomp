108a7398  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a739c  0050a0e1  cpy r5, r0
108a73a0  0240a0e1  cpy r4, r2
108a73a4  0300a0e3  mov r0, #0x3
108a73a8  5b11ffeb  bl 0x1086b91c
108a73ac  040050e1  cmp r0, r4
108a73b0  0900001a  bne 0x108a73dc
108a73b4  1600a0e3  mov r0, #0x16
108a73b8  5711ffeb  bl 0x1086b91c
108a73bc  110050e3  cmp r0, #0x11
108a73c0  0500001a  bne 0x108a73dc
108a73c4  0500a0e1  cpy r0, r5
108a73c8  0596feeb  bl 0x1084cbe4
108a73cc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a73d0  0010a0e1  cpy r1, r0
108a73d4  3200a0e3  mov r0, #0x32
108a73d8  a996feea  b 0x1084ce84
108a73dc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
