108bca9c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bcaa0  0150a0e1  cpy r5, r1
108bcaa4  0240a0e1  cpy r4, r2
108bcaa8  0300a0e3  mov r0, #0x3
108bcaac  9abbfeeb  bl 0x1086b91c
108bcab0  040050e1  cmp r0, r4
108bcab4  1300001a  bne 0x108bcb08
108bcab8  0410a0e1  cpy r1, r4
108bcabc  0500a0e1  cpy r0, r5
108bcac0  765efeeb  bl 0x108544a0
108bcac4  0050a0e1  cpy r5, r0
108bcac8  0310a0e3  mov r1, #0x3
108bcacc  943dfeeb  bl 0x1084c124
108bcad0  0040a0e1  cpy r4, r0
108bcad4  0d10a0e3  mov r1, #0xd
108bcad8  0500a0e1  cpy r0, r5
108bcadc  4fdefeeb  bl 0x10874420
108bcae0  040050e1  cmp r0, r4
108bcae4  0700008a  bhi 0x108bcb08
108bcae8  1600a0e3  mov r0, #0x16
108bcaec  8abbfeeb  bl 0x1086b91c
108bcaf0  090050e3  cmp r0, #0x9
108bcaf4  0300001a  bne 0x108bcb08
108bcaf8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bcafc  061ba0e3  mov r1, #0x1800
108bcb00  3600a0e3  mov r0, #0x36
108bcb04  de40feea  b 0x1084ce84
108bcb08  7080bde8  ldmia sp!,{r4,r5,r6,pc}
