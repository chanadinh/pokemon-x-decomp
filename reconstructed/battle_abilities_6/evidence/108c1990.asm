108c1990  10402de9  stmdb sp!,{r4,lr}
108c1994  0240a0e1  cpy r4, r2
108c1998  0400a0e3  mov r0, #0x4
108c199c  dea7feeb  bl 0x1086b91c
108c19a0  040050e1  cmp r0, r4
108c19a4  0300001a  bne 0x108c19b8
108c19a8  1040bde8  ldmia sp!,{r4,lr}
108c19ac  0110a0e3  mov r1, #0x1
108c19b0  4200a0e3  mov r0, #0x42
108c19b4  c68ffeea  b 0x108658d4
108c19b8  1080bde8  ldmia sp!,{r4,pc}
