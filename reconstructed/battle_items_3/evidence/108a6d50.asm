108a6d50  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6d54  0050a0e1  cpy r5, r0
108a6d58  0240a0e1  cpy r4, r2
108a6d5c  0300a0e3  mov r0, #0x3
108a6d60  ed12ffeb  bl 0x1086b91c
108a6d64  040050e1  cmp r0, r4
108a6d68  0900001a  bne 0x108a6d94
108a6d6c  1600a0e3  mov r0, #0x16
108a6d70  e912ffeb  bl 0x1086b91c
108a6d74  0a0050e3  cmp r0, #0xa
108a6d78  0500001a  bne 0x108a6d94
108a6d7c  0500a0e1  cpy r0, r5
108a6d80  9797feeb  bl 0x1084cbe4
108a6d84  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6d88  0010a0e1  cpy r1, r0
108a6d8c  3200a0e3  mov r0, #0x32
108a6d90  3b98feea  b 0x1084ce84
108a6d94  7080bde8  ldmia sp!,{r4,r5,r6,pc}
