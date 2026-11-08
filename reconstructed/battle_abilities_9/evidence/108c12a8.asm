108c12a8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c12ac  0050a0e1  cpy r5, r0
108c12b0  0240a0e1  cpy r4, r2
108c12b4  0200a0e3  mov r0, #0x2
108c12b8  97a9feeb  bl 0x1086b91c
108c12bc  040050e1  cmp r0, r4
108c12c0  0200001a  bne 0x108c12d0
108c12c4  0500a0e1  cpy r0, r5
108c12c8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c12cc  0627feea  b 0x1084aeec
108c12d0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
