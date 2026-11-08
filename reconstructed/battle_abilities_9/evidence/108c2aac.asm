108c2aac  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2ab0  0060a0e1  cpy r6, r0
108c2ab4  0250a0e1  cpy r5, r2
108c2ab8  0340a0e1  cpy r4, r3
108c2abc  0200a0e3  mov r0, #0x2
108c2ac0  95a3feeb  bl 0x1086b91c
108c2ac4  050050e1  cmp r0, r5
108c2ac8  00009405  ldreq r0, [r4,#0x0]
108c2acc  01005003  cmpeq r0, #0x1
108c2ad0  0300001a  bne 0x108c2ae4
108c2ad4  0600a0e1  cpy r0, r6
108c2ad8  0321feeb  bl 0x1084aeec
108c2adc  0000a0e3  mov r0, #0x0
108c2ae0  000084e5  str r0, [r4,#0x0]
108c2ae4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
