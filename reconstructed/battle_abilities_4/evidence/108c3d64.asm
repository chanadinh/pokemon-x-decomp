108c3d64  10402de9  stmdb sp!,{r4,lr}
108c3d68  0240a0e1  cpy r4, r2
108c3d6c  0300a0e3  mov r0, #0x3
108c3d70  e99efeeb  bl 0x1086b91c
108c3d74  040050e1  cmp r0, r4
108c3d78  0300001a  bne 0x108c3d8c
108c3d7c  1040bde8  ldmia sp!,{r4,lr}
108c3d80  0110a0e3  mov r1, #0x1
108c3d84  4c00a0e3  mov r0, #0x4c
108c3d88  d186feea  b 0x108658d4
108c3d8c  1080bde8  ldmia sp!,{r4,pc}
