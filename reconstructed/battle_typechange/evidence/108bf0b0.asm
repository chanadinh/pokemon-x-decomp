108bf0b0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bf0b4  0000a0e3  mov r0, #0x0
108bf0b8  000083e5  str r0, [r3,#0x0]
108bf0bc  0340a0e1  cpy r4, r3
108bf0c0  0250a0e1  cpy r5, r2
108bf0c4  1160a0e3  mov r6, #0x11
108bf0c8  0200a0e3  mov r0, #0x2
108bf0cc  12b2feeb  bl 0x1086b91c
108bf0d0  050050e1  cmp r0, r5
108bf0d4  0700001a  bne 0x108bf0f8
108bf0d8  1600a0e3  mov r0, #0x16
108bf0dc  0eb2feeb  bl 0x1086b91c
108bf0e0  000050e3  cmp r0, #0x0
108bf0e4  0300001a  bne 0x108bf0f8
108bf0e8  0610a0e1  cpy r1, r6
108bf0ec  1600a0e3  mov r0, #0x16
108bf0f0  f799feeb  bl 0x108658d4
108bf0f4  000084e5  str r0, [r4,#0x0]
108bf0f8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
