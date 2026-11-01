108c4368  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c436c  0150a0e1  cpy r5, r1
108c4370  0240a0e1  cpy r4, r2
108c4374  0360a0e3  mov r6, #0x3
108c4378  0200a0e3  mov r0, #0x2
108c437c  669dfeeb  bl 0x1086b91c
108c4380  040050e1  cmp r0, r4
108c4384  0400001a  bne 0x108c439c
108c4388  0620a0e1  cpy r2, r6
108c438c  0410a0e1  cpy r1, r4
108c4390  0500a0e1  cpy r0, r5
108c4394  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c4398  011afeea  b 0x1084aba4
108c439c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
