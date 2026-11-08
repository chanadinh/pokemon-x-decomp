108c239c  10402de9  stmdb sp!,{r4,lr}
108c23a0  0240a0e1  cpy r4, r2
108c23a4  0400a0e3  mov r0, #0x4
108c23a8  5ba5feeb  bl 0x1086b91c
108c23ac  040050e1  cmp r0, r4
108c23b0  0300001a  bne 0x108c23c4
108c23b4  1040bde8  ldmia sp!,{r4,lr}
108c23b8  0110a0e3  mov r1, #0x1
108c23bc  5300a0e3  mov r0, #0x53
108c23c0  438dfeea  b 0x108658d4
108c23c4  1080bde8  ldmia sp!,{r4,pc}
