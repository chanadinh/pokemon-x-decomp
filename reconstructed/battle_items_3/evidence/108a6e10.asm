108a6e10  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6e14  0050a0e1  cpy r5, r0
108a6e18  0240a0e1  cpy r4, r2
108a6e1c  0300a0e3  mov r0, #0x3
108a6e20  bd12ffeb  bl 0x1086b91c
108a6e24  040050e1  cmp r0, r4
108a6e28  0900001a  bne 0x108a6e54
108a6e2c  1600a0e3  mov r0, #0x16
108a6e30  b912ffeb  bl 0x1086b91c
108a6e34  050050e3  cmp r0, #0x5
108a6e38  0500001a  bne 0x108a6e54
108a6e3c  0500a0e1  cpy r0, r5
108a6e40  6797feeb  bl 0x1084cbe4
108a6e44  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6e48  0010a0e1  cpy r1, r0
108a6e4c  3200a0e3  mov r0, #0x32
108a6e50  0b98feea  b 0x1084ce84
108a6e54  7080bde8  ldmia sp!,{r4,r5,r6,pc}
