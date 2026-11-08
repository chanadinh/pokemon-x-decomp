108af710  10402de9  stmdb sp!,{r4,lr}
108af714  0240a0e1  cpy r4, r2
108af718  0300a0e3  mov r0, #0x3
108af71c  7ef0feeb  bl 0x1086b91c
108af720  040050e1  cmp r0, r4
108af724  0500001a  bne 0x108af740
108af728  0400a0e3  mov r0, #0x4
108af72c  7af0feeb  bl 0x1086b91c
108af730  ff1000e2  and r1, r0, #0xff
108af734  1040bde8  ldmia sp!,{r4,lr}
108af738  3c00a0e3  mov r0, #0x3c
108af73c  64d8feea  b 0x108658d4
108af740  1080bde8  ldmia sp!,{r4,pc}
