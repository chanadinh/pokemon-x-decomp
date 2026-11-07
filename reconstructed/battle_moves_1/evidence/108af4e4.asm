108af4e4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108af4e8  0150a0e1  cpy r5, r1
108af4ec  0240a0e1  cpy r4, r2
108af4f0  0300a0e3  mov r0, #0x3
108af4f4  08f1feeb  bl 0x1086b91c
108af4f8  040050e1  cmp r0, r4
108af4fc  0e00001a  bne 0x108af53c
108af500  0400a0e3  mov r0, #0x4
108af504  04f1feeb  bl 0x1086b91c
108af508  ff1000e2  and r1, r0, #0xff
108af50c  0500a0e1  cpy r0, r5
108af510  e293feeb  bl 0x108544a0
108af514  0210a0e3  mov r1, #0x2
108af518  be74feeb  bl 0x1084c818
108af51c  000050e3  cmp r0, #0x0
108af520  0500000a  beq 0x108af53c
108af524  3100a0e3  mov r0, #0x31
108af528  fbf0feeb  bl 0x1086b91c
108af52c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108af530  8010a0e1  mov r1, r0, lsl #0x1
108af534  3100a0e3  mov r0, #0x31
108af538  e5d8feea  b 0x108658d4
108af53c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
