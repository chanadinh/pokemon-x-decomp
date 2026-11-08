108a7db4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7db8  0350a0e1  cpy r5, r3
108a7dbc  0160a0e1  cpy r6, r1
108a7dc0  0240a0e1  cpy r4, r2
108a7dc4  0300a0e3  mov r0, #0x3
108a7dc8  d30effeb  bl 0x1086b91c
108a7dcc  040050e1  cmp r0, r4
108a7dd0  0e00001a  bne 0x108a7e10
108a7dd4  1600a0e3  mov r0, #0x16
108a7dd8  cf0effeb  bl 0x1086b91c
108a7ddc  070050e3  cmp r0, #0x7
108a7de0  0a00001a  bne 0x108a7e10
108a7de4  000095e5  ldr r0, [r5,#0x0]
108a7de8  010050e3  cmp r0, #0x1
108a7dec  0300000a  beq 0x108a7e00
108a7df0  0600a0e1  cpy r0, r6
108a7df4  4492feeb  bl 0x1084c70c
108a7df8  000050e3  cmp r0, #0x0
108a7dfc  0300000a  beq 0x108a7e10
108a7e00  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7e04  3200a0e3  mov r0, #0x32
108a7e08  04109fe5  ldr r1, [0x108a7e14]
108a7e0c  1c94feea  b 0x1084ce84
108a7e10  7080bde8  ldmia sp!,{r4,r5,r6,pc}
