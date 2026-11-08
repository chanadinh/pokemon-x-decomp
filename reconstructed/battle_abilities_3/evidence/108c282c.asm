108c282c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2830  0240a0e1  cpy r4, r2
108c2834  0350a0e1  cpy r5, r3
108c2838  0200a0e3  mov r0, #0x2
108c283c  36a4feeb  bl 0x1086b91c
108c2840  040050e1  cmp r0, r4
108c2844  0000a003  moveq r0, #0x0
108c2848  00008505  streq r0, [r5,#0x0]
108c284c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
