108af540  10402de9  stmdb sp!,{r4,lr}
108af544  0240a0e1  cpy r4, r2
108af548  0300a0e3  mov r0, #0x3
108af54c  f2f0feeb  bl 0x1086b91c
108af550  040050e1  cmp r0, r4
108af554  0300001a  bne 0x108af568
108af558  1040bde8  ldmia sp!,{r4,lr}
108af55c  0110a0e3  mov r1, #0x1
108af560  5300a0e3  mov r0, #0x53
108af564  dad8feea  b 0x108658d4
108af568  1080bde8  ldmia sp!,{r4,pc}
