108bb1f0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb1f4  0150a0e1  cpy r5, r1
108bb1f8  0240a0e1  cpy r4, r2
108bb1fc  0300a0e3  mov r0, #0x3
108bb200  c5c1feeb  bl 0x1086b91c
108bb204  040050e1  cmp r0, r4
108bb208  0c00001a  bne 0x108bb240
108bb20c  0400a0e3  mov r0, #0x4
108bb210  c1c1feeb  bl 0x1086b91c
108bb214  ff1000e2  and r1, r0, #0xff
108bb218  0500a0e1  cpy r0, r5
108bb21c  9f64feeb  bl 0x108544a0
108bb220  0810a0e3  mov r1, #0x8
108bb224  0846feeb  bl 0x1084ca4c
108bb228  000050e3  cmp r0, #0x0
108bb22c  0300000a  beq 0x108bb240
108bb230  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb234  0110a0e3  mov r1, #0x1
108bb238  5300a0e3  mov r0, #0x53
108bb23c  a4a9feea  b 0x108658d4
108bb240  7080bde8  ldmia sp!,{r4,r5,r6,pc}
