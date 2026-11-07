108b6f0c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b6f10  0150a0e1  cpy r5, r1
108b6f14  0240a0e1  cpy r4, r2
108b6f18  0300a0e3  mov r0, #0x3
108b6f1c  7ed2feeb  bl 0x1086b91c
108b6f20  040050e1  cmp r0, r4
108b6f24  0e00001a  bne 0x108b6f64
108b6f28  0400a0e3  mov r0, #0x4
108b6f2c  7ad2feeb  bl 0x1086b91c
108b6f30  ff1000e2  and r1, r0, #0xff
108b6f34  0500a0e1  cpy r0, r5
108b6f38  5875feeb  bl 0x108544a0
108b6f3c  0210a0e3  mov r1, #0x2
108b6f40  d07efeeb  bl 0x10856a88
108b6f44  000050e3  cmp r0, #0x0
108b6f48  0500000a  beq 0x108b6f64
108b6f4c  3100a0e3  mov r0, #0x31
108b6f50  71d2feeb  bl 0x1086b91c
108b6f54  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b6f58  8010a0e1  mov r1, r0, lsl #0x1
108b6f5c  3100a0e3  mov r0, #0x31
108b6f60  5bbafeea  b 0x108658d4
108b6f64  7080bde8  ldmia sp!,{r4,r5,r6,pc}
