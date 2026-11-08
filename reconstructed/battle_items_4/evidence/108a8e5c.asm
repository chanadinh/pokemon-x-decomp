108a8e5c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8e60  0350a0e1  cpy r5, r3
108a8e64  0160a0e1  cpy r6, r1
108a8e68  0240a0e1  cpy r4, r2
108a8e6c  0300a0e3  mov r0, #0x3
108a8e70  a90affeb  bl 0x1086b91c
108a8e74  040050e1  cmp r0, r4
108a8e78  0e00001a  bne 0x108a8eb8
108a8e7c  1600a0e3  mov r0, #0x16
108a8e80  a50affeb  bl 0x1086b91c
108a8e84  020050e3  cmp r0, #0x2
108a8e88  0a00001a  bne 0x108a8eb8
108a8e8c  000095e5  ldr r0, [r5,#0x0]
108a8e90  010050e3  cmp r0, #0x1
108a8e94  0300000a  beq 0x108a8ea8
108a8e98  0600a0e1  cpy r0, r6
108a8e9c  1a8efeeb  bl 0x1084c70c
108a8ea0  000050e3  cmp r0, #0x0
108a8ea4  0300000a  beq 0x108a8eb8
108a8ea8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8eac  3200a0e3  mov r0, #0x32
108a8eb0  04109fe5  ldr r1, [0x108a8ebc]
108a8eb4  f28ffeea  b 0x1084ce84
108a8eb8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
