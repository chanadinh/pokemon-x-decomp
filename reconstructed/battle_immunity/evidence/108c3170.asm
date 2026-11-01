108c3170  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c3174  0140a0e1  cpy r4, r1
108c3178  0250a0e1  cpy r5, r2
108c317c  0a20a0e3  mov r2, #0xa
108c3180  0510a0e1  cpy r1, r5
108c3184  0400a0e1  cpy r0, r4
108c3188  a589feeb  bl 0x10865824
108c318c  000050e3  cmp r0, #0x0
108c3190  0400000a  beq 0x108c31a8
108c3194  0510a0e1  cpy r1, r5
108c3198  0400a0e1  cpy r0, r4
108c319c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c31a0  0420a0e3  mov r2, #0x4
108c31a4  401efeea  b 0x1084aaac
108c31a8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
