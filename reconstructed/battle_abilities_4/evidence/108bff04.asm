108bff04  10402de9  stmdb sp!,{r4,lr}
108bff08  0240a0e1  cpy r4, r2
108bff0c  0400a0e3  mov r0, #0x4
108bff10  81aefeeb  bl 0x1086b91c
108bff14  040050e1  cmp r0, r4
108bff18  0300001a  bne 0x108bff2c
108bff1c  1040bde8  ldmia sp!,{r4,lr}
108bff20  0010a0e3  mov r1, #0x0
108bff24  5300a0e3  mov r0, #0x53
108bff28  6996feea  b 0x108658d4
108bff2c  1080bde8  ldmia sp!,{r4,pc}
