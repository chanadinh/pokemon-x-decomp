108c09f8  10402de9  stmdb sp!,{r4,lr}
108c09fc  0240a0e1  cpy r4, r2
108c0a00  0400a0e3  mov r0, #0x4
108c0a04  c4abfeeb  bl 0x1086b91c
108c0a08  040050e1  cmp r0, r4
108c0a0c  0300001a  bne 0x108c0a20
108c0a10  1040bde8  ldmia sp!,{r4,lr}
108c0a14  0110a0e3  mov r1, #0x1
108c0a18  4200a0e3  mov r0, #0x42
108c0a1c  ac93feea  b 0x108658d4
108c0a20  1080bde8  ldmia sp!,{r4,pc}
