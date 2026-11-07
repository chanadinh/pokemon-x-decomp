108af494  70402de9  stmdb sp!,{r4,r5,r6,lr}
108af498  0150a0e1  cpy r5, r1
108af49c  0240a0e1  cpy r4, r2
108af4a0  0300a0e3  mov r0, #0x3
108af4a4  1cf1feeb  bl 0x1086b91c
108af4a8  040050e1  cmp r0, r4
108af4ac  0b00001a  bne 0x108af4e0
108af4b0  0410a0e1  cpy r1, r4
108af4b4  0500a0e1  cpy r0, r5
108af4b8  f893feeb  bl 0x108544a0
108af4bc  b201d0e1  ldrh r0, [r0,#0x12]
108af4c0  000050e3  cmp r0, #0x0
108af4c4  0500001a  bne 0x108af4e0
108af4c8  3100a0e3  mov r0, #0x31
108af4cc  12f1feeb  bl 0x1086b91c
108af4d0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108af4d4  8010a0e1  mov r1, r0, lsl #0x1
108af4d8  3100a0e3  mov r0, #0x31
108af4dc  fcd8feea  b 0x108658d4
108af4e0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
