108aa578  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aa57c  0160a0e1  cpy r6, r1
108aa580  0250a0e1  cpy r5, r2
108aa584  0340a0e1  cpy r4, r3
108aa588  0400a0e3  mov r0, #0x4
108aa58c  e204ffeb  bl 0x1086b91c
108aa590  050050e1  cmp r0, r5
108aa594  0900001a  bne 0x108aa5c0
108aa598  0510a0e1  cpy r1, r5
108aa59c  0600a0e1  cpy r0, r6
108aa5a0  bea7feeb  bl 0x108544a0
108aa5a4  5b8bfeeb  bl 0x1084d318
108aa5a8  000050e3  cmp r0, #0x0
108aa5ac  0200000a  beq 0x108aa5bc
108aa5b0  0310a0e3  mov r1, #0x3
108aa5b4  3b00a0e3  mov r0, #0x3b
108aa5b8  c5ecfeeb  bl 0x108658d4
108aa5bc  000084e5  str r0, [r4,#0x0]
108aa5c0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
