108c1fbc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c1fc0  0060a0e1  cpy r6, r0
108c1fc4  0250a0e1  cpy r5, r2
108c1fc8  0340a0e1  cpy r4, r3
108c1fcc  0200a0e3  mov r0, #0x2
108c1fd0  51a6feeb  bl 0x1086b91c
108c1fd4  050050e1  cmp r0, r5
108c1fd8  00009405  ldreq r0, [r4,#0x0]
108c1fdc  01005003  cmpeq r0, #0x1
108c1fe0  0300001a  bne 0x108c1ff4
108c1fe4  0600a0e1  cpy r0, r6
108c1fe8  bf23feeb  bl 0x1084aeec
108c1fec  0000a0e3  mov r0, #0x0
108c1ff0  000084e5  str r0, [r4,#0x0]
108c1ff4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
