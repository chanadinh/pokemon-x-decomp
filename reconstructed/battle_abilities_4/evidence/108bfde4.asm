108bfde4  10402de9  stmdb sp!,{r4,lr}
108bfde8  0240a0e1  cpy r4, r2
108bfdec  0200a0e3  mov r0, #0x2
108bfdf0  c9aefeeb  bl 0x1086b91c
108bfdf4  040050e1  cmp r0, r4
108bfdf8  0300001a  bne 0x108bfe0c
108bfdfc  1040bde8  ldmia sp!,{r4,lr}
108bfe00  021aa0e3  mov r1, #0x2000
108bfe04  3600a0e3  mov r0, #0x36
108bfe08  1d34feea  b 0x1084ce84
108bfe0c  1080bde8  ldmia sp!,{r4,pc}
