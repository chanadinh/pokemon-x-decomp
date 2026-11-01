108c06c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c06cc  0150a0e1  cpy r5, r1
108c06d0  0240a0e1  cpy r4, r2
108c06d4  0160a0e3  mov r6, #0x1
108c06d8  0200a0e3  mov r0, #0x2
108c06dc  8eacfeeb  bl 0x1086b91c
108c06e0  040050e1  cmp r0, r4
108c06e4  0400001a  bne 0x108c06fc
108c06e8  0620a0e1  cpy r2, r6
108c06ec  0410a0e1  cpy r1, r4
108c06f0  0500a0e1  cpy r0, r5
108c06f4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c06f8  2929feea  b 0x1084aba4
108c06fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
