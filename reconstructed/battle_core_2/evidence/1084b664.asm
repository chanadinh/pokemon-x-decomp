1084b664  010010e3  tst r0, #0x1
1084b668  0100a013  movne r0, #0x1
1084b66c  8100a001  moveq r0, r1, lsl #0x1
1084b670  81008010  addne r0, r0, r1, lsl #0x1
1084b674  ff0000e2  and r0, r0, #0xff
1084b678  1eff2fe1  bx lr
