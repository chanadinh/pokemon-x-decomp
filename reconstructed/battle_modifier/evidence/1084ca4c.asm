1084ca4c  012c80e2  add r2, r0, #0x100
1084ca50  692082e2  add r2, r2, #0x69
1084ca54  a101a0e1  mov r0, r1, lsr #0x3
1084ca58  071001e2  and r1, r1, #0x7
1084ca5c  0020d2e7  ldrb r2, [r2,r0]
1084ca60  0130a0e3  mov r3, #0x1
1084ca64  1301a0e1  mov r0, r3, lsl r1
1084ca68  ff0000e2  and r0, r0, #0xff
1084ca6c  020010e0  ands r0, r0, r2
1084ca70  0100a013  movne r0, #0x1
1084ca74  1eff2fe1  bx lr
