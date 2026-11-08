108a8d78  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8d7c  0350a0e1  cpy r5, r3
108a8d80  0160a0e1  cpy r6, r1
108a8d84  0240a0e1  cpy r4, r2
108a8d88  0300a0e3  mov r0, #0x3
108a8d8c  e20affeb  bl 0x1086b91c
108a8d90  040050e1  cmp r0, r4
108a8d94  0e00001a  bne 0x108a8dd4
108a8d98  1600a0e3  mov r0, #0x16
108a8d9c  de0affeb  bl 0x1086b91c
108a8da0  0c0050e3  cmp r0, #0xc
108a8da4  0a00001a  bne 0x108a8dd4
108a8da8  000095e5  ldr r0, [r5,#0x0]
108a8dac  010050e3  cmp r0, #0x1
108a8db0  0300000a  beq 0x108a8dc4
108a8db4  0600a0e1  cpy r0, r6
108a8db8  538efeeb  bl 0x1084c70c
108a8dbc  000050e3  cmp r0, #0x0
108a8dc0  0300000a  beq 0x108a8dd4
108a8dc4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8dc8  3200a0e3  mov r0, #0x32
108a8dcc  04109fe5  ldr r1, [0x108a8dd8]
108a8dd0  2b90feea  b 0x1084ce84
108a8dd4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
