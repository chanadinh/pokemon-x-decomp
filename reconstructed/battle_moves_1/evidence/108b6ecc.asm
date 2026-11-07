108b6ecc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b6ed0  0250a0e1  cpy r5, r2
108b6ed4  0340a0e1  cpy r4, r3
108b6ed8  0300a0e3  mov r0, #0x3
108b6edc  8ed2feeb  bl 0x1086b91c
108b6ee0  050050e1  cmp r0, r5
108b6ee4  0700001a  bne 0x108b6f08
108b6ee8  000094e5  ldr r0, [r4,#0x0]
108b6eec  000050e3  cmp r0, #0x0
108b6ef0  0a00a003  moveq r0, #0xa
108b6ef4  00008405  streq r0, [r4,#0x0]
108b6ef8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b6efc  0010a0e1  cpy r1, r0
108b6f00  3100a0e3  mov r0, #0x31
108b6f04  72bafeea  b 0x108658d4
108b6f08  7080bde8  ldmia sp!,{r4,r5,r6,pc}
