108aace0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aace4  0060a0e1  cpy r6, r0
108aace8  0150a0e1  cpy r5, r1
108aacec  0240a0e1  cpy r4, r2
108aacf0  0200a0e3  mov r0, #0x2
108aacf4  0803ffeb  bl 0x1086b91c
108aacf8  040050e1  cmp r0, r4
108aacfc  0c00001a  bne 0x108aad34
108aad00  0410a0e1  cpy r1, r4
108aad04  0500a0e1  cpy r0, r5
108aad08  e4a5feeb  bl 0x108544a0
108aad0c  0b85feeb  bl 0x1084c140
108aad10  000050e3  cmp r0, #0x0
108aad14  0600000a  beq 0x108aad34
108aad18  0600a0e1  cpy r0, r6
108aad1c  1287feeb  bl 0x1084c96c
108aad20  0420a0e1  cpy r2, r4
108aad24  0500a0e1  cpy r0, r5
108aad28  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aad2c  0010a0e3  mov r1, #0x0
108aad30  0487feea  b 0x1084c948
108aad34  7080bde8  ldmia sp!,{r4,r5,r6,pc}
