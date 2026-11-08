108a8ec0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8ec4  0350a0e1  cpy r5, r3
108a8ec8  0160a0e1  cpy r6, r1
108a8ecc  0240a0e1  cpy r4, r2
108a8ed0  0300a0e3  mov r0, #0x3
108a8ed4  900affeb  bl 0x1086b91c
108a8ed8  040050e1  cmp r0, r4
108a8edc  0e00001a  bne 0x108a8f1c
108a8ee0  1600a0e3  mov r0, #0x16
108a8ee4  8c0affeb  bl 0x1086b91c
108a8ee8  090050e3  cmp r0, #0x9
108a8eec  0a00001a  bne 0x108a8f1c
108a8ef0  000095e5  ldr r0, [r5,#0x0]
108a8ef4  010050e3  cmp r0, #0x1
108a8ef8  0300000a  beq 0x108a8f0c
108a8efc  0600a0e1  cpy r0, r6
108a8f00  018efeeb  bl 0x1084c70c
108a8f04  000050e3  cmp r0, #0x0
108a8f08  0300000a  beq 0x108a8f1c
108a8f0c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8f10  3200a0e3  mov r0, #0x32
108a8f14  04109fe5  ldr r1, [0x108a8f20]
108a8f18  d98ffeea  b 0x1084ce84
108a8f1c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
