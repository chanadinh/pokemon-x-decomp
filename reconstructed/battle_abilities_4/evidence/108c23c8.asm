108c23c8  10402de9  stmdb sp!,{r4,lr}
108c23cc  0240a0e1  cpy r4, r2
108c23d0  0300a0e3  mov r0, #0x3
108c23d4  50a5feeb  bl 0x1086b91c
108c23d8  040050e1  cmp r0, r4
108c23dc  0300001a  bne 0x108c23f0
108c23e0  1040bde8  ldmia sp!,{r4,lr}
108c23e4  0110a0e3  mov r1, #0x1
108c23e8  5300a0e3  mov r0, #0x53
108c23ec  388dfeea  b 0x108658d4
108c23f0  1080bde8  ldmia sp!,{r4,pc}
