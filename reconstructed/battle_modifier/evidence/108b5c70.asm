108b5c70  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b5c74  0150a0e1  cpy r5, r1
108b5c78  0240a0e1  cpy r4, r2
108b5c7c  0300a0e3  mov r0, #0x3
108b5c80  25d7feeb  bl 0x1086b91c
108b5c84  040050e1  cmp r0, r4
108b5c88  0c00001a  bne 0x108b5cc0
108b5c8c  0400a0e3  mov r0, #0x4
108b5c90  21d7feeb  bl 0x1086b91c
108b5c94  ff1000e2  and r1, r0, #0xff
108b5c98  0500a0e1  cpy r0, r5
108b5c9c  ff79feeb  bl 0x108544a0
108b5ca0  0510a0e3  mov r1, #0x5
108b5ca4  685bfeeb  bl 0x1084ca4c
108b5ca8  000050e3  cmp r0, #0x0
108b5cac  0300000a  beq 0x108b5cc0
108b5cb0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b5cb4  021aa0e3  mov r1, #0x2000
108b5cb8  3600a0e3  mov r0, #0x36
108b5cbc  705cfeea  b 0x1084ce84
108b5cc0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
