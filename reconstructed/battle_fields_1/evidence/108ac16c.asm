108ac16c  10402de9  stmdb sp!,{r4,lr}
108ac170  0240a0e1  cpy r4, r2
108ac174  0200a0e3  mov r0, #0x2
108ac178  e7fdfeeb  bl 0x1086b91c
108ac17c  ff0000e2  and r0, r0, #0xff
108ac180  8e7ffeeb  bl 0x1084bfc0
108ac184  040050e1  cmp r0, r4
108ac188  0300001a  bne 0x108ac19c
108ac18c  1040bde8  ldmia sp!,{r4,lr}
108ac190  021aa0e3  mov r1, #0x2000
108ac194  3600a0e3  mov r0, #0x36
108ac198  3983feea  b 0x1084ce84
108ac19c  1080bde8  ldmia sp!,{r4,pc}
