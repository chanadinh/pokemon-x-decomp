108b7ebc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b7ec0  0160a0e1  cpy r6, r1
108b7ec4  0240a0e1  cpy r4, r2
108b7ec8  0350a0e1  cpy r5, r3
108b7ecc  0200a0e3  mov r0, #0x2
108b7ed0  91cefeeb  bl 0x1086b91c
108b7ed4  040050e1  cmp r0, r4
108b7ed8  00009505  ldreq r0, [r5,#0x0]
108b7edc  00005003  cmpeq r0, #0x0
108b7ee0  1000001a  bne 0x108b7f28
108b7ee4  0410a0e1  cpy r1, r4
108b7ee8  0600a0e1  cpy r0, r6
108b7eec  6b71feeb  bl 0x108544a0
108b7ef0  b201d0e1  ldrh r0, [r0,#0x12]
108b7ef4  000050e3  cmp r0, #0x0
108b7ef8  0a00000a  beq 0x108b7f28
108b7efc  0420a0e1  cpy r2, r4
108b7f00  2410a0e3  mov r1, #0x24
108b7f04  0600a0e1  cpy r0, r6
108b7f08  1bc8feeb  bl 0x10869f7c
108b7f0c  0120a0e3  mov r2, #0x1
108b7f10  0420c0e5  strb r2, [r0,#0x4]
108b7f14  0010a0e1  cpy r1, r0
108b7f18  0520c0e5  strb r2, [r0,#0x5]
108b7f1c  0600a0e1  cpy r0, r6
108b7f20  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7f24  07bffeea  b 0x10867b48
108b7f28  7080bde8  ldmia sp!,{r4,r5,r6,pc}
