108a7b98  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7b9c  0050a0e1  cpy r5, r0
108a7ba0  0240a0e1  cpy r4, r2
108a7ba4  0300a0e3  mov r0, #0x3
108a7ba8  5b0fffeb  bl 0x1086b91c
108a7bac  040050e1  cmp r0, r4
108a7bb0  0900001a  bne 0x108a7bdc
108a7bb4  1600a0e3  mov r0, #0x16
108a7bb8  570fffeb  bl 0x1086b91c
108a7bbc  0e0050e3  cmp r0, #0xe
108a7bc0  0500001a  bne 0x108a7bdc
108a7bc4  0500a0e1  cpy r0, r5
108a7bc8  0594feeb  bl 0x1084cbe4
108a7bcc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7bd0  0010a0e1  cpy r1, r0
108a7bd4  3200a0e3  mov r0, #0x32
108a7bd8  a994feea  b 0x1084ce84
108a7bdc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
