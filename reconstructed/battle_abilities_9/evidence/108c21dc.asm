1084aef8  0c1080e5  str r1, [r0,#0xc]
1084aefc  1eff2fe1  bx lr
108c21dc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c21e0  0050a0e1  cpy r5, r0
108c21e4  0240a0e1  cpy r4, r2
108c21e8  0300a0e3  mov r0, #0x3
108c21ec  caa5feeb  bl 0x1086b91c
108c21f0  040050e1  cmp r0, r4
108c21f4  0300001a  bne 0x108c2208
108c21f8  0500a0e1  cpy r0, r5
108c21fc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c2200  04109fe5  ldr r1, [0x108c220c]
108c2204  3b23feea  b 0x1084aef8
108c2208  7080bde8  ldmia sp!,{r4,r5,r6,pc}
