108af994  70402de9  stmdb sp!,{r4,r5,r6,lr}
108af998  0150a0e1  cpy r5, r1
108af99c  0240a0e1  cpy r4, r2
108af9a0  0300a0e3  mov r0, #0x3
108af9a4  dceffeeb  bl 0x1086b91c
108af9a8  040050e1  cmp r0, r4
108af9ac  0d00001a  bne 0x108af9e8
108af9b0  0410a0e1  cpy r1, r4
108af9b4  0500a0e1  cpy r0, r5
108af9b8  b892feeb  bl 0x108544a0
108af9bc  000090e5  ldr r0, [r0,#0x0]
108af9c0  cb48fdeb  bl 0x10801cf4
108af9c4  001180e0  add r1, r0, r0, lsl #0x2
108af9c8  1c009fe5  ldr r0, [0x108af9ec]
108af9cc  8110a0e1  mov r1, r1, lsl #0x1
108af9d0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108af9d4  901180e0  umull r1, r0, r0, r1
108af9d8  a011b0e1  movs r1, r0, lsr #0x3
108af9dc  0110a003  moveq r1, #0x1
108af9e0  3100a0e3  mov r0, #0x31
108af9e4  bad7feea  b 0x108658d4
108af9e8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
