108a6c2c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6c30  0050a0e1  cpy r5, r0
108a6c34  0240a0e1  cpy r4, r2
108a6c38  0300a0e3  mov r0, #0x3
108a6c3c  3613ffeb  bl 0x1086b91c
108a6c40  040050e1  cmp r0, r4
108a6c44  0900001a  bne 0x108a6c70
108a6c48  1600a0e3  mov r0, #0x16
108a6c4c  3213ffeb  bl 0x1086b91c
108a6c50  0f0050e3  cmp r0, #0xf
108a6c54  0500001a  bne 0x108a6c70
108a6c58  0500a0e1  cpy r0, r5
108a6c5c  e097feeb  bl 0x1084cbe4
108a6c60  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6c64  0010a0e1  cpy r1, r0
108a6c68  3200a0e3  mov r0, #0x32
108a6c6c  8498feea  b 0x1084ce84
108a6c70  7080bde8  ldmia sp!,{r4,r5,r6,pc}
