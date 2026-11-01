108c15a8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c15ac  0350a0e1  cpy r5, r3
108c15b0  0240a0e1  cpy r4, r2
108c15b4  0300a0e3  mov r0, #0x3
108c15b8  d7a8feeb  bl 0x1086b91c
108c15bc  040050e1  cmp r0, r4
108c15c0  0a00001a  bne 0x108c15f0
108c15c4  1600a0e3  mov r0, #0x16
108c15c8  d3a8feeb  bl 0x1086b91c
108c15cc  110050e3  cmp r0, #0x11
108c15d0  0600001a  bne 0x108c15f0
108c15d4  000095e5  ldr r0, [r5,#0x0]
108c15d8  000050e3  cmp r0, #0x0
108c15dc  0300000a  beq 0x108c15f0
108c15e0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c15e4  3200a0e3  mov r0, #0x32
108c15e8  04109fe5  ldr r1, [0x108c15f4]
108c15ec  242efeea  b 0x1084ce84
108c15f0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
