108bde48  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bde4c  0000a0e3  mov r0, #0x0
108bde50  0260a0e3  mov r6, #0x2
108bde54  000083e5  str r0, [r3,#0x0]
108bde58  0340a0e1  cpy r4, r3
108bde5c  0250a0e1  cpy r5, r2
108bde60  0600a0e1  cpy r0, r6
108bde64  acb6feeb  bl 0x1086b91c
108bde68  050050e1  cmp r0, r5
108bde6c  0700001a  bne 0x108bde90
108bde70  1600a0e3  mov r0, #0x16
108bde74  a8b6feeb  bl 0x1086b91c
108bde78  000050e3  cmp r0, #0x0
108bde7c  0300001a  bne 0x108bde90
108bde80  0610a0e1  cpy r1, r6
108bde84  1600a0e3  mov r0, #0x16
108bde88  919efeeb  bl 0x108658d4
108bde8c  000084e5  str r0, [r4,#0x0]
108bde90  7080bde8  ldmia sp!,{r4,r5,r6,pc}
