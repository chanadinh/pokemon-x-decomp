108bf8cc  10402de9  stmdb sp!,{r4,lr}
108bf8d0  0240a0e1  cpy r4, r2
108bf8d4  0300a0e3  mov r0, #0x3
108bf8d8  0fb0feeb  bl 0x1086b91c
108bf8dc  040050e1  cmp r0, r4
108bf8e0  0300001a  bne 0x108bf8f4
108bf8e4  1040bde8  ldmia sp!,{r4,lr}
108bf8e8  0110a0e3  mov r1, #0x1
108bf8ec  5300a0e3  mov r0, #0x53
108bf8f0  f797feea  b 0x108658d4
108bf8f4  1080bde8  ldmia sp!,{r4,pc}
