1084c818  012c80e2  add r2, r0, #0x100
1084c81c  662082e2  add r2, r2, #0x66
1084c820  a101a0e1  mov r0, r1, lsr #0x3
1084c824  071001e2  and r1, r1, #0x7
1084c828  0020d2e7  ldrb r2, [r2,r0]
1084c82c  0130a0e3  mov r3, #0x1
1084c830  1301a0e1  mov r0, r3, lsl r1
1084c834  ff0000e2  and r0, r0, #0xff
1084c838  020010e0  ands r0, r0, r2
1084c83c  0100a013  movne r0, #0x1
1084c840  1eff2fe1  bx lr
