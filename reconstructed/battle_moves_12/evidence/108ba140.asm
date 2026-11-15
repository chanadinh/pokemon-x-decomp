108ba140  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba144  0150a0e1  cpy r5, r1
108ba148  0260a0e1  cpy r6, r2
108ba14c  0340a0e1  cpy r4, r3
108ba150  0300a0e3  mov r0, #0x3
108ba154  f0c5feeb  bl 0x1086b91c
108ba158  060050e1  cmp r0, r6
108ba15c  0800001a  bne 0x108ba184
108ba160  001094e5  ldr r1, [r4,#0x0]
108ba164  0500a0e1  cpy r0, r5
108ba168  000051e3  cmp r1, #0x0
108ba16c  0400000a  beq 0x108ba184
108ba170  041094e5  ldr r1, [r4,#0x4]
108ba174  ff1001e2  and r1, r1, #0xff
108ba178  8343feeb  bl 0x1084af8c
108ba17c  0000a0e3  mov r0, #0x0
108ba180  000084e5  str r0, [r4,#0x0]
108ba184  7080bde8  ldmia sp!,{r4,r5,r6,pc}
