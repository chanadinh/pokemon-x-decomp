108c4c80  10402de9  stmdb sp!,{r4,lr}
108c4c84  001093e5  ldr r1, [r3,#0x0]
108c4c88  0340a0e1  cpy r4, r3
108c4c8c  000051e3  cmp r1, #0x0
108c4c90  0300001a  bne 0x108c4ca4
108c4c94  0c109fe5  ldr r1, [0x108c4ca8]
108c4c98  0c1080e5  str r1, [r0,#0xc]
108c4c9c  0100a0e3  mov r0, #0x1
108c4ca0  000084e5  str r0, [r4,#0x0]
108c4ca4  1080bde8  ldmia sp!,{r4,pc}
