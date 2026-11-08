108c11c0  10402de9  stmdb sp!,{r4,lr}
108c11c4  0240a0e1  cpy r4, r2
108c11c8  0400a0e3  mov r0, #0x4
108c11cc  d2a9feeb  bl 0x1086b91c
108c11d0  040050e1  cmp r0, r4
108c11d4  0300001a  bne 0x108c11e8
108c11d8  1040bde8  ldmia sp!,{r4,lr}
108c11dc  0110a0e3  mov r1, #0x1
108c11e0  4800a0e3  mov r0, #0x48
108c11e4  ba91feea  b 0x108658d4
108c11e8  1080bde8  ldmia sp!,{r4,pc}
