108c3a18  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c3a1c  0160a0e1  cpy r6, r1
108c3a20  0250a0e1  cpy r5, r2
108c3a24  0340a0e1  cpy r4, r3
108c3a28  0400a0e3  mov r0, #0x4
108c3a2c  ba9ffeeb  bl 0x1086b91c
108c3a30  050050e1  cmp r0, r5
108c3a34  0900001a  bne 0x108c3a60
108c3a38  0510a0e1  cpy r1, r5
108c3a3c  0600a0e1  cpy r0, r6
108c3a40  9642feeb  bl 0x108544a0
108c3a44  3326feeb  bl 0x1084d318
108c3a48  000050e3  cmp r0, #0x0
108c3a4c  0200000a  beq 0x108c3a5c
108c3a50  0410a0e3  mov r1, #0x4
108c3a54  3b00a0e3  mov r0, #0x3b
108c3a58  9d87feeb  bl 0x108658d4
108c3a5c  000084e5  str r0, [r4,#0x0]
108c3a60  7080bde8  ldmia sp!,{r4,r5,r6,pc}
