108a5e28  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a5e2c  0050a0e1  cpy r5, r0
108a5e30  0240a0e1  cpy r4, r2
108a5e34  0300a0e3  mov r0, #0x3
108a5e38  b716ffeb  bl 0x1086b91c
108a5e3c  040050e1  cmp r0, r4
108a5e40  0900001a  bne 0x108a5e6c
108a5e44  1600a0e3  mov r0, #0x16
108a5e48  b316ffeb  bl 0x1086b91c
108a5e4c  090050e3  cmp r0, #0x9
108a5e50  0500001a  bne 0x108a5e6c
108a5e54  0500a0e1  cpy r0, r5
108a5e58  619bfeeb  bl 0x1084cbe4
108a5e5c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a5e60  0010a0e1  cpy r1, r0
108a5e64  3200a0e3  mov r0, #0x32
108a5e68  059cfeea  b 0x1084ce84
108a5e6c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
