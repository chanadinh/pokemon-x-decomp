108a7ea8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7eac  0350a0e1  cpy r5, r3
108a7eb0  0160a0e1  cpy r6, r1
108a7eb4  0240a0e1  cpy r4, r2
108a7eb8  0300a0e3  mov r0, #0x3
108a7ebc  960effeb  bl 0x1086b91c
108a7ec0  040050e1  cmp r0, r4
108a7ec4  0e00001a  bne 0x108a7f04
108a7ec8  1600a0e3  mov r0, #0x16
108a7ecc  920effeb  bl 0x1086b91c
108a7ed0  050050e3  cmp r0, #0x5
108a7ed4  0a00001a  bne 0x108a7f04
108a7ed8  000095e5  ldr r0, [r5,#0x0]
108a7edc  010050e3  cmp r0, #0x1
108a7ee0  0300000a  beq 0x108a7ef4
108a7ee4  0600a0e1  cpy r0, r6
108a7ee8  0792feeb  bl 0x1084c70c
108a7eec  000050e3  cmp r0, #0x0
108a7ef0  0300000a  beq 0x108a7f04
108a7ef4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7ef8  3200a0e3  mov r0, #0x32
108a7efc  04109fe5  ldr r1, [0x108a7f08]
108a7f00  df93feea  b 0x1084ce84
108a7f04  7080bde8  ldmia sp!,{r4,r5,r6,pc}
