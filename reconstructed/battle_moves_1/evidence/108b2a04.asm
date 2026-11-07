108b2a04  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b2a08  0150a0e1  cpy r5, r1
108b2a0c  0240a0e1  cpy r4, r2
108b2a10  0300a0e3  mov r0, #0x3
108b2a14  c0e3feeb  bl 0x1086b91c
108b2a18  040050e1  cmp r0, r4
108b2a1c  0900001a  bne 0x108b2a48
108b2a20  0500a0e1  cpy r0, r5
108b2a24  7d65feeb  bl 0x1084c020
108b2a28  000050e3  cmp r0, #0x0
108b2a2c  0500000a  beq 0x108b2a48
108b2a30  3100a0e3  mov r0, #0x31
108b2a34  b8e3feeb  bl 0x1086b91c
108b2a38  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b2a3c  8010a0e1  mov r1, r0, lsl #0x1
108b2a40  3100a0e3  mov r0, #0x31
108b2a44  a2cbfeea  b 0x108658d4
108b2a48  7080bde8  ldmia sp!,{r4,r5,r6,pc}
