108b3c6c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b3c70  0150a0e1  cpy r5, r1
108b3c74  0240a0e1  cpy r4, r2
108b3c78  0200a0e3  mov r0, #0x2
108b3c7c  26dffeeb  bl 0x1086b91c
108b3c80  040050e1  cmp r0, r4
108b3c84  0a00001a  bne 0x108b3cb4
108b3c88  0500a0e1  cpy r0, r5
108b3c8c  0410a0e1  cpy r1, r4
108b3c90  0282feeb  bl 0x108544a0
108b3c94  0010a0e3  mov r1, #0x0
108b3c98  6b63feeb  bl 0x1084ca4c
108b3c9c  000050e3  cmp r0, #0x0
108b3ca0  0300000a  beq 0x108b3cb4
108b3ca4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b3ca8  1b10a0e3  mov r1, #0x1b
108b3cac  2300a0e3  mov r0, #0x23
108b3cb0  07c7feea  b 0x108658d4
108b3cb4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
