108c07b0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c07b4  0150a0e1  cpy r5, r1
108c07b8  0240a0e1  cpy r4, r2
108c07bc  0660a0e3  mov r6, #0x6
108c07c0  0200a0e3  mov r0, #0x2
108c07c4  54acfeeb  bl 0x1086b91c
108c07c8  040050e1  cmp r0, r4
108c07cc  0400001a  bne 0x108c07e4
108c07d0  0620a0e1  cpy r2, r6
108c07d4  0410a0e1  cpy r1, r4
108c07d8  0500a0e1  cpy r0, r5
108c07dc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c07e0  ef28feea  b 0x1084aba4
108c07e4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
