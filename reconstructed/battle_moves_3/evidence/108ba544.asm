108ba544  10402de9  stmdb sp!,{r4,lr}
108ba548  0240a0e1  cpy r4, r2
108ba54c  0300a0e3  mov r0, #0x3
108ba550  f1c4feeb  bl 0x1086b91c
108ba554  040050e1  cmp r0, r4
108ba558  0300001a  bne 0x108ba56c
108ba55c  1040bde8  ldmia sp!,{r4,lr}
108ba560  0110a0e3  mov r1, #0x1
108ba564  4c00a0e3  mov r0, #0x4c
108ba568  d9acfeea  b 0x108658d4
108ba56c  1080bde8  ldmia sp!,{r4,pc}
