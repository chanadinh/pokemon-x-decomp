108c0114  10402de9  stmdb sp!,{r4,lr}
108c0118  0240a0e1  cpy r4, r2
108c011c  0200a0e3  mov r0, #0x2
108c0120  fdadfeeb  bl 0x1086b91c
108c0124  040050e1  cmp r0, r4
108c0128  0300001a  bne 0x108c013c
108c012c  1040bde8  ldmia sp!,{r4,lr}
108c0130  0010a0e3  mov r1, #0x0
108c0134  1600a0e3  mov r0, #0x16
108c0138  e595feea  b 0x108658d4
108c013c  1080bde8  ldmia sp!,{r4,pc}
