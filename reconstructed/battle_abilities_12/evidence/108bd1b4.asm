108bd1b4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd1b8  0250a0e1  cpy r5, r2
108bd1bc  0340a0e1  cpy r4, r3
108bd1c0  0200a0e3  mov r0, #0x2
108bd1c4  d4b9feeb  bl 0x1086b91c
108bd1c8  050050e1  cmp r0, r5
108bd1cc  0a00001a  bne 0x108bd1fc
108bd1d0  000094e5  ldr r0, [r4,#0x0]
108bd1d4  000050e3  cmp r0, #0x0
108bd1d8  0100a003  moveq r0, #0x1
108bd1dc  00008405  streq r0, [r4,#0x0]
108bd1e0  0500000a  beq 0x108bd1fc
108bd1e4  1310a0e3  mov r1, #0x13
108bd1e8  2300a0e3  mov r0, #0x23
108bd1ec  b8a1feeb  bl 0x108658d4
108bd1f0  0010a0e3  mov r1, #0x0
108bd1f4  040084e5  str r0, [r4,#0x4]
108bd1f8  001084e5  str r1, [r4,#0x0]
108bd1fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
