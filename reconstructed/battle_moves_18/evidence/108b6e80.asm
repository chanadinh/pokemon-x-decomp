108b6e80  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b6e84  0240a0e1  cpy r4, r2
108b6e88  0350a0e1  cpy r5, r3
108b6e8c  0400a0e3  mov r0, #0x4
108b6e90  a1d2feeb  bl 0x1086b91c
108b6e94  040050e1  cmp r0, r4
108b6e98  0a00001a  bne 0x108b6ec8
108b6e9c  180095e5  ldr r0, [r5,#0x18]
108b6ea0  000050e3  cmp r0, #0x0
108b6ea4  0700000a  beq 0x108b6ec8
108b6ea8  5300a0e3  mov r0, #0x53
108b6eac  9ad2feeb  bl 0x1086b91c
108b6eb0  000050e3  cmp r0, #0x0
108b6eb4  0300000a  beq 0x108b6ec8
108b6eb8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b6ebc  0110a0e3  mov r1, #0x1
108b6ec0  3b00a0e3  mov r0, #0x3b
108b6ec4  82bafeea  b 0x108658d4
108b6ec8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
