108c3fe0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c3fe4  0050a0e1  cpy r5, r0
108c3fe8  0260a0e1  cpy r6, r2
108c3fec  0340a0e1  cpy r4, r3
108c3ff0  0200a0e3  mov r0, #0x2
108c3ff4  489efeeb  bl 0x1086b91c
108c3ff8  060050e1  cmp r0, r6
108c3ffc  0700001a  bne 0x108c4020
108c4000  001094e5  ldr r1, [r4,#0x0]
108c4004  0500a0e1  cpy r0, r5
108c4008  000051e3  cmp r1, #0x0
108c400c  0300001a  bne 0x108c4020
108c4010  0c109fe5  ldr r1, [0x108c4024]
108c4014  0c1080e5  str r1, [r0,#0xc]
108c4018  0100a0e3  mov r0, #0x1
108c401c  000084e5  str r0, [r4,#0x0]
108c4020  7080bde8  ldmia sp!,{r4,r5,r6,pc}
