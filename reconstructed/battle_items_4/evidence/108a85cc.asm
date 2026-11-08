108a85cc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a85d0  0250a0e1  cpy r5, r2
108a85d4  0340a0e1  cpy r4, r3
108a85d8  0300a0e3  mov r0, #0x3
108a85dc  ce0cffeb  bl 0x1086b91c
108a85e0  050050e1  cmp r0, r5
108a85e4  00009405  ldreq r0, [r4,#0x0]
108a85e8  01005003  cmpeq r0, #0x1
108a85ec  0200a003  moveq r0, #0x2
108a85f0  00008405  streq r0, [r4,#0x0]
108a85f4  ffffff1a  bne 0x108a85f8
108a85f8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
