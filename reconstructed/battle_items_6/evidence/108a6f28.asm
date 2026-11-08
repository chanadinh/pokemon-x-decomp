108a6f28  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6f2c  0050a0e1  cpy r5, r0
108a6f30  0240a0e1  cpy r4, r2
108a6f34  0300a0e3  mov r0, #0x3
108a6f38  7712ffeb  bl 0x1086b91c
108a6f3c  040050e1  cmp r0, r4
108a6f40  0500001a  bne 0x108a6f5c
108a6f44  0500a0e1  cpy r0, r5
108a6f48  2597feeb  bl 0x1084cbe4
108a6f4c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6f50  0010a0e1  cpy r1, r0
108a6f54  3600a0e3  mov r0, #0x36
108a6f58  c997feea  b 0x1084ce84
108a6f5c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
