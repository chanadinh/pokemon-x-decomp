108afa98  70402de9  stmdb sp!,{r4,r5,r6,lr}
108afa9c  0160a0e1  cpy r6, r1
108afaa0  0250a0e1  cpy r5, r2
108afaa4  0300a0e3  mov r0, #0x3
108afaa8  9beffeeb  bl 0x1086b91c
108afaac  050050e1  cmp r0, r5
108afab0  1400001a  bne 0x108afb08
108afab4  0510a0e1  cpy r1, r5
108afab8  0600a0e1  cpy r0, r6
108afabc  7792feeb  bl 0x108544a0
108afac0  79f7feeb  bl 0x1086d8ac
108afac4  050050e3  cmp r0, #0x5
108afac8  0040a0e1  cpy r4, r0
108afacc  01005413  cmpne r4, #0x1
108afad0  04005413  cmpne r4, #0x4
108afad4  0b00001a  bne 0x108afb08
108afad8  0520a0e1  cpy r2, r5
108afadc  0a10a0e3  mov r1, #0xa
108afae0  0600a0e1  cpy r0, r6
108afae4  24e9feeb  bl 0x10869f7c
108afae8  0010a0e1  cpy r1, r0
108afaec  0100a0e3  mov r0, #0x1
108afaf0  0440c1e5  strb r4, [r1,#0x4]
108afaf4  1100c1e5  strb r0, [r1,#0x11]
108afaf8  0600a0e1  cpy r0, r6
108afafc  0550c1e5  strb r5, [r1,#0x5]
108afb00  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108afb04  0fe0feea  b 0x10867b48
108afb08  7080bde8  ldmia sp!,{r4,r5,r6,pc}
