108c3724  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c3728  0050a0e1  cpy r5, r0
108c372c  0240a0e1  cpy r4, r2
108c3730  0200a0e3  mov r0, #0x2
108c3734  78a0feeb  bl 0x1086b91c
108c3738  040050e1  cmp r0, r4
108c373c  0200001a  bne 0x108c374c
108c3740  0500a0e1  cpy r0, r5
108c3744  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c3748  e71dfeea  b 0x1084aeec
108c374c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
