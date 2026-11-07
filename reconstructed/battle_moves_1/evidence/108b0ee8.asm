108b0ee8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b0eec  0150a0e1  cpy r5, r1
108b0ef0  0240a0e1  cpy r4, r2
108b0ef4  0300a0e3  mov r0, #0x3
108b0ef8  87eafeeb  bl 0x1086b91c
108b0efc  040050e1  cmp r0, r4
108b0f00  0d00001a  bne 0x108b0f3c
108b0f04  0400a0e3  mov r0, #0x4
108b0f08  83eafeeb  bl 0x1086b91c
108b0f0c  ff1000e2  and r1, r0, #0xff
108b0f10  0500a0e1  cpy r0, r5
108b0f14  618dfeeb  bl 0x108544a0
108b0f18  63f2feeb  bl 0x1086d8ac
108b0f1c  000050e3  cmp r0, #0x0
108b0f20  0500000a  beq 0x108b0f3c
108b0f24  3100a0e3  mov r0, #0x31
108b0f28  7beafeeb  bl 0x1086b91c
108b0f2c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b0f30  8010a0e1  mov r1, r0, lsl #0x1
108b0f34  3100a0e3  mov r0, #0x31
108b0f38  65d2feea  b 0x108658d4
108b0f3c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
