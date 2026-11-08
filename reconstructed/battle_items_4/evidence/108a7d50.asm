108a7d50  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7d54  0350a0e1  cpy r5, r3
108a7d58  0160a0e1  cpy r6, r1
108a7d5c  0240a0e1  cpy r4, r2
108a7d60  0300a0e3  mov r0, #0x3
108a7d64  ec0effeb  bl 0x1086b91c
108a7d68  040050e1  cmp r0, r4
108a7d6c  0e00001a  bne 0x108a7dac
108a7d70  1600a0e3  mov r0, #0x16
108a7d74  e80effeb  bl 0x1086b91c
108a7d78  0d0050e3  cmp r0, #0xd
108a7d7c  0a00001a  bne 0x108a7dac
108a7d80  000095e5  ldr r0, [r5,#0x0]
108a7d84  010050e3  cmp r0, #0x1
108a7d88  0300000a  beq 0x108a7d9c
108a7d8c  0600a0e1  cpy r0, r6
108a7d90  5d92feeb  bl 0x1084c70c
108a7d94  000050e3  cmp r0, #0x0
108a7d98  0300000a  beq 0x108a7dac
108a7d9c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7da0  3200a0e3  mov r0, #0x32
108a7da4  04109fe5  ldr r1, [0x108a7db0]
108a7da8  3594feea  b 0x1084ce84
108a7dac  7080bde8  ldmia sp!,{r4,r5,r6,pc}
