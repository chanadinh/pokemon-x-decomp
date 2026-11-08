108a7cd0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7cd4  0350a0e1  cpy r5, r3
108a7cd8  0160a0e1  cpy r6, r1
108a7cdc  0240a0e1  cpy r4, r2
108a7ce0  0300a0e3  mov r0, #0x3
108a7ce4  0c0fffeb  bl 0x1086b91c
108a7ce8  040050e1  cmp r0, r4
108a7cec  0e00001a  bne 0x108a7d2c
108a7cf0  1600a0e3  mov r0, #0x16
108a7cf4  080fffeb  bl 0x1086b91c
108a7cf8  100050e3  cmp r0, #0x10
108a7cfc  0a00001a  bne 0x108a7d2c
108a7d00  000095e5  ldr r0, [r5,#0x0]
108a7d04  010050e3  cmp r0, #0x1
108a7d08  0300000a  beq 0x108a7d1c
108a7d0c  0600a0e1  cpy r0, r6
108a7d10  7d92feeb  bl 0x1084c70c
108a7d14  000050e3  cmp r0, #0x0
108a7d18  0300000a  beq 0x108a7d2c
108a7d1c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7d20  3200a0e3  mov r0, #0x32
108a7d24  04109fe5  ldr r1, [0x108a7d30]
108a7d28  5594feea  b 0x1084ce84
108a7d2c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
