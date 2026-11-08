108bf554  10402de9  stmdb sp!,{r4,lr}
108bf558  0240a0e1  cpy r4, r2
108bf55c  0300a0e3  mov r0, #0x3
108bf560  edb0feeb  bl 0x1086b91c
108bf564  040050e1  cmp r0, r4
108bf568  0700001a  bne 0x108bf58c
108bf56c  1500a0e3  mov r0, #0x15
108bf570  e9b0feeb  bl 0x1086b91c
108bf574  070050e3  cmp r0, #0x7
108bf578  0300001a  bne 0x108bf58c
108bf57c  1040bde8  ldmia sp!,{r4,lr}
108bf580  0110a0e3  mov r1, #0x1
108bf584  4c00a0e3  mov r0, #0x4c
108bf588  d198feea  b 0x108658d4
108bf58c  1080bde8  ldmia sp!,{r4,pc}
