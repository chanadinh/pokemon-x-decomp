108c08b8  10402de9  stmdb sp!,{r4,lr}
108c08bc  0240a0e1  cpy r4, r2
108c08c0  0400a0e3  mov r0, #0x4
108c08c4  14acfeeb  bl 0x1086b91c
108c08c8  040050e1  cmp r0, r4
108c08cc  0700001a  bne 0x108c08f0
108c08d0  0300a0e3  mov r0, #0x3
108c08d4  10acfeeb  bl 0x1086b91c
108c08d8  040050e1  cmp r0, r4
108c08dc  0300000a  beq 0x108c08f0
108c08e0  1040bde8  ldmia sp!,{r4,lr}
108c08e4  0110a0e3  mov r1, #0x1
108c08e8  4200a0e3  mov r0, #0x42
108c08ec  f893feea  b 0x108658d4
108c08f0  1080bde8  ldmia sp!,{r4,pc}
