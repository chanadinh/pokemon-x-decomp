108b9f9c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9fa0  0250a0e1  cpy r5, r2
108b9fa4  0340a0e1  cpy r4, r3
108b9fa8  0200a0e3  mov r0, #0x2
108b9fac  5ac6feeb  bl 0x1086b91c
108b9fb0  050050e1  cmp r0, r5
108b9fb4  0f00001a  bne 0x108b9ff8
108b9fb8  000094e5  ldr r0, [r4,#0x0]
108b9fbc  000050e3  cmp r0, #0x0
108b9fc0  0c00000a  beq 0x108b9ff8
108b9fc4  040094e5  ldr r0, [r4,#0x4]
108b9fc8  0010a0e3  mov r1, #0x0
108b9fcc  010050e3  cmp r0, #0x1
108b9fd0  24109f05  ldreq r1, [0x108b9ffc]
108b9fd4  0400000a  beq 0x108b9fec
108b9fd8  020050e3  cmp r0, #0x2
108b9fdc  1c109f05  ldreq r1, [0x108ba000]
108b9fe0  0100000a  beq 0x108b9fec
108b9fe4  030050e3  cmp r0, #0x3
108b9fe8  821fa003  moveq r1, #0x208
108b9fec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9ff0  1300a0e3  mov r0, #0x13
108b9ff4  36aefeea  b 0x108658d4
108b9ff8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
