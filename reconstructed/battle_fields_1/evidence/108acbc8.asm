108acbc8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108acbcc  0250a0e1  cpy r5, r2
108acbd0  0360a0e1  cpy r6, r3
108acbd4  0200a0e3  mov r0, #0x2
108acbd8  4ffbfeeb  bl 0x1086b91c
108acbdc  ff4000e2  and r4, r0, #0xff
108acbe0  0400a0e1  cpy r0, r4
108acbe4  f57cfeeb  bl 0x1084bfc0
108acbe8  050050e1  cmp r0, r5
108acbec  0b00001a  bne 0x108acc20
108acbf0  2100a0e3  mov r0, #0x21
108acbf4  48fbfeeb  bl 0x1086b91c
108acbf8  000050e3  cmp r0, #0x0
108acbfc  070000aa  bge 0x108acc20
108acc00  0300a0e3  mov r0, #0x3
108acc04  44fbfeeb  bl 0x1086b91c
108acc08  040050e1  cmp r0, r4
108acc0c  0300000a  beq 0x108acc20
108acc10  0110a0e3  mov r1, #0x1
108acc14  4200a0e3  mov r0, #0x42
108acc18  2de3feeb  bl 0x108658d4
108acc1c  000086e5  str r0, [r6,#0x0]
108acc20  7080bde8  ldmia sp!,{r4,r5,r6,pc}
