108b4cdc  10402de9  stmdb sp!,{r4,lr}
108b4ce0  0240a0e1  cpy r4, r2
108b4ce4  0300a0e3  mov r0, #0x3
108b4ce8  0bdbfeeb  bl 0x1086b91c
108b4cec  040050e1  cmp r0, r4
108b4cf0  0300001a  bne 0x108b4d04
108b4cf4  1040bde8  ldmia sp!,{r4,lr}
108b4cf8  0210a0e3  mov r1, #0x2
108b4cfc  1700a0e3  mov r0, #0x17
108b4d00  f3c2feea  b 0x108658d4
108b4d04  1080bde8  ldmia sp!,{r4,pc}
