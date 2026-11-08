108c1ebc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c1ec0  0050a0e1  cpy r5, r0
108c1ec4  0240a0e1  cpy r4, r2
108c1ec8  0200a0e3  mov r0, #0x2
108c1ecc  92a6feeb  bl 0x1086b91c
108c1ed0  040050e1  cmp r0, r4
108c1ed4  0200001a  bne 0x108c1ee4
108c1ed8  0500a0e1  cpy r0, r5
108c1edc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c1ee0  062afeea  b 0x1084c700
108c1ee4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
