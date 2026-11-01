108bf0fc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bf100  0000a0e3  mov r0, #0x0
108bf104  000083e5  str r0, [r3,#0x0]
108bf108  0340a0e1  cpy r4, r3
108bf10c  0250a0e1  cpy r5, r2
108bf110  0e60a0e3  mov r6, #0xe
108bf114  0200a0e3  mov r0, #0x2
108bf118  ffb1feeb  bl 0x1086b91c
108bf11c  050050e1  cmp r0, r5
108bf120  0700001a  bne 0x108bf144
108bf124  1600a0e3  mov r0, #0x16
108bf128  fbb1feeb  bl 0x1086b91c
108bf12c  000050e3  cmp r0, #0x0
108bf130  0300001a  bne 0x108bf144
108bf134  0610a0e1  cpy r1, r6
108bf138  1600a0e3  mov r0, #0x16
108bf13c  e499feeb  bl 0x108658d4
108bf140  000084e5  str r0, [r4,#0x0]
108bf144  7080bde8  ldmia sp!,{r4,r5,r6,pc}
