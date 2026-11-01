108bf300  10402de9  stmdb sp!,{r4,lr}
108bf304  0240a0e1  cpy r4, r2
108bf308  0300a0e3  mov r0, #0x3
108bf30c  82b1feeb  bl 0x1086b91c
108bf310  040050e1  cmp r0, r4
108bf314  0900001a  bne 0x108bf340
108bf318  3900a0e3  mov r0, #0x39
108bf31c  7eb1feeb  bl 0x1086b91c
108bf320  ff0000e2  and r0, r0, #0xff
108bf324  987cffeb  bl 0x1089e58c
108bf328  030050e3  cmp r0, #0x3
108bf32c  0300001a  bne 0x108bf340
108bf330  1040bde8  ldmia sp!,{r4,lr}
108bf334  021aa0e3  mov r1, #0x2000
108bf338  3600a0e3  mov r0, #0x36
108bf33c  d036feea  b 0x1084ce84
108bf340  1080bde8  ldmia sp!,{r4,pc}
