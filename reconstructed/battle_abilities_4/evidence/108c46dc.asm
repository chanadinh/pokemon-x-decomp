108c46dc  10402de9  stmdb sp!,{r4,lr}
108c46e0  0240a0e1  cpy r4, r2
108c46e4  0300a0e3  mov r0, #0x3
108c46e8  8b9cfeeb  bl 0x1086b91c
108c46ec  040050e1  cmp r0, r4
108c46f0  0300001a  bne 0x108c4704
108c46f4  1040bde8  ldmia sp!,{r4,lr}
108c46f8  0110a0e3  mov r1, #0x1
108c46fc  4600a0e3  mov r0, #0x46
108c4700  7384feea  b 0x108658d4
108c4704  1080bde8  ldmia sp!,{r4,pc}
