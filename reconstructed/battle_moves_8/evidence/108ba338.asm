108ba338  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba33c  0150a0e1  cpy r5, r1
108ba340  0240a0e1  cpy r4, r2
108ba344  0300a0e3  mov r0, #0x3
108ba348  73c5feeb  bl 0x1086b91c
108ba34c  040050e1  cmp r0, r4
108ba350  0c00001a  bne 0x108ba388
108ba354  0400a0e3  mov r0, #0x4
108ba358  6fc5feeb  bl 0x1086b91c
108ba35c  ff1000e2  and r1, r0, #0xff
108ba360  0500a0e1  cpy r0, r5
108ba364  4d68feeb  bl 0x108544a0
108ba368  0710a0e3  mov r1, #0x7
108ba36c  7bd5feeb  bl 0x1086f960
108ba370  000050e3  cmp r0, #0x0
108ba374  0300000a  beq 0x108ba388
108ba378  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba37c  0110a0e3  mov r1, #0x1
108ba380  4100a0e3  mov r0, #0x41
108ba384  52adfeea  b 0x108658d4
108ba388  7080bde8  ldmia sp!,{r4,r5,r6,pc}
