1084b648  000090e5  ldr r0, [r0,#0x0]
1084b64c  0f1000e2  and r1, r0, #0xf
1084b650  010051e3  cmp r1, #0x1
1084b654  0004a001  moveq r0, r0, lsl #0x8
1084b658  2008a001  moveq r0, r0, lsr #0x10
1084b65c  0000a013  movne r0, #0x0
1084b660  1eff2fe1  bx lr
