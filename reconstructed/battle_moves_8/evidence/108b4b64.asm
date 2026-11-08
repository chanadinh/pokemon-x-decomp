108b4b64  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b4b68  0150a0e1  cpy r5, r1
108b4b6c  0240a0e1  cpy r4, r2
108b4b70  0300a0e3  mov r0, #0x3
108b4b74  68dbfeeb  bl 0x1086b91c
108b4b78  040050e1  cmp r0, r4
108b4b7c  0c00001a  bne 0x108b4bb4
108b4b80  0400a0e3  mov r0, #0x4
108b4b84  64dbfeeb  bl 0x1086b91c
108b4b88  ff1000e2  and r1, r0, #0xff
108b4b8c  0500a0e1  cpy r0, r5
108b4b90  427efeeb  bl 0x108544a0
108b4b94  0210a0e3  mov r1, #0x2
108b4b98  ba87feeb  bl 0x10856a88
108b4b9c  000050e3  cmp r0, #0x0
108b4ba0  0300001a  bne 0x108b4bb4
108b4ba4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b4ba8  0110a0e3  mov r1, #0x1
108b4bac  4100a0e3  mov r0, #0x41
108b4bb0  47c3feea  b 0x108658d4
108b4bb4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
