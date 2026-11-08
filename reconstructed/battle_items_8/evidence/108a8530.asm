108a8530  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8534  0150a0e1  cpy r5, r1
108a8538  0240a0e1  cpy r4, r2
108a853c  0200a0e3  mov r0, #0x2
108a8540  f50cffeb  bl 0x1086b91c
108a8544  040050e1  cmp r0, r4
108a8548  0d00001a  bne 0x108a8584
108a854c  0420a0e1  cpy r2, r4
108a8550  0d10a0e3  mov r1, #0xd
108a8554  0500a0e1  cpy r0, r5
108a8558  8706ffeb  bl 0x10869f7c
108a855c  0010a0e1  cpy r1, r0
108a8560  0120a0e3  mov r2, #0x1
108a8564  0400a0e3  mov r0, #0x4
108a8568  1620c1e5  strb r2, [r1,#0x16]
108a856c  0400c1e5  strb r0, [r1,#0x4]
108a8570  1720c1e5  strb r2, [r1,#0x17]
108a8574  0500a0e1  cpy r0, r5
108a8578  0c40c1e5  strb r4, [r1,#0xc]
108a857c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8580  70fdfeea  b 0x10867b48
108a8584  7080bde8  ldmia sp!,{r4,r5,r6,pc}
