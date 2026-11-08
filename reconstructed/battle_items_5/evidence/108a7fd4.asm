108a7fd4  10402de9  stmdb sp!,{r4,lr}
108a7fd8  0240a0e1  cpy r4, r2
108a7fdc  0200a0e3  mov r0, #0x2
108a7fe0  4d0effeb  bl 0x1086b91c
108a7fe4  040050e1  cmp r0, r4
108a7fe8  0300001a  bne 0x108a7ffc
108a7fec  1040bde8  ldmia sp!,{r4,lr}
108a7ff0  061ba0e3  mov r1, #0x1800
108a7ff4  3600a0e3  mov r0, #0x36
108a7ff8  a193feea  b 0x1084ce84
108a7ffc  1080bde8  ldmia sp!,{r4,pc}
