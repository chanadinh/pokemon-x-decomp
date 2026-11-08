108c3cd0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c3cd4  0240a0e1  cpy r4, r2
108c3cd8  0350a0e1  cpy r5, r3
108c3cdc  0200a0e3  mov r0, #0x2
108c3ce0  0d9ffeeb  bl 0x1086b91c
108c3ce4  040050e1  cmp r0, r4
108c3ce8  0600001a  bne 0x108c3d08
108c3cec  0c0095e5  ldr r0, [r5,#0xc]
108c3cf0  000050e3  cmp r0, #0x0
108c3cf4  0300000a  beq 0x108c3d08
108c3cf8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c3cfc  021ba0e3  mov r1, #0x800
108c3d00  3600a0e3  mov r0, #0x36
108c3d04  5e24feea  b 0x1084ce84
108c3d08  7080bde8  ldmia sp!,{r4,r5,r6,pc}
