108c5698  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c569c  0140a0e1  cpy r4, r1
108c56a0  0250a0e1  cpy r5, r2
108c56a4  0a20a0e3  mov r2, #0xa
108c56a8  0510a0e1  cpy r1, r5
108c56ac  0400a0e1  cpy r0, r4
108c56b0  5b80feeb  bl 0x10865824
108c56b4  000050e3  cmp r0, #0x0
108c56b8  0500000a  beq 0x108c56d4
108c56bc  0510a0e1  cpy r1, r5
108c56c0  0400a0e1  cpy r0, r4
108c56c4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c56c8  0130a0e3  mov r3, #0x1
108c56cc  0320a0e3  mov r2, #0x3
108c56d0  ec88feea  b 0x10867a88
108c56d4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
