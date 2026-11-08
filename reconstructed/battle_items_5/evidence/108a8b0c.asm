108a8b0c  10402de9  stmdb sp!,{r4,lr}
108a8b10  0240a0e1  cpy r4, r2
108a8b14  0400a0e3  mov r0, #0x4
108a8b18  7f0bffeb  bl 0x1086b91c
108a8b1c  040050e1  cmp r0, r4
108a8b20  0700001a  bne 0x108a8b44
108a8b24  1b00a0e3  mov r0, #0x1b
108a8b28  7b0bffeb  bl 0x1086b91c
108a8b2c  020050e3  cmp r0, #0x2
108a8b30  0300001a  bne 0x108a8b44
108a8b34  1040bde8  ldmia sp!,{r4,lr}
108a8b38  061ba0e3  mov r1, #0x1800
108a8b3c  3600a0e3  mov r0, #0x36
108a8b40  cf90feea  b 0x1084ce84
108a8b44  1080bde8  ldmia sp!,{r4,pc}
