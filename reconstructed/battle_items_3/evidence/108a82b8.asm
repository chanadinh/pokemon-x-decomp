108a82b8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a82bc  0050a0e1  cpy r5, r0
108a82c0  0240a0e1  cpy r4, r2
108a82c4  0300a0e3  mov r0, #0x3
108a82c8  930dffeb  bl 0x1086b91c
108a82cc  040050e1  cmp r0, r4
108a82d0  0900001a  bne 0x108a82fc
108a82d4  1600a0e3  mov r0, #0x16
108a82d8  8f0dffeb  bl 0x1086b91c
108a82dc  040050e3  cmp r0, #0x4
108a82e0  0500001a  bne 0x108a82fc
108a82e4  0500a0e1  cpy r0, r5
108a82e8  3d92feeb  bl 0x1084cbe4
108a82ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a82f0  0010a0e1  cpy r1, r0
108a82f4  3200a0e3  mov r0, #0x32
108a82f8  e192feea  b 0x1084ce84
108a82fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
