108acedc  0100a0e1  cpy r0, r1
108acee0  0210a0e1  cpy r1, r2
108acee4  0000a0e1  cpy r0, r0
108acee8  14209fe5  ldr r2, [0x108acf04]
108aceec  000280e0  add r0, r0, r0, lsl #0x4
108acef0  000282e0  add r0, r2, r0, lsl #0x4
108acef4  010290e7  ldr r0, [r0,r1,lsl #0x4]
108acef8  000050e3  cmp r0, #0x0
108acefc  0100a013  movne r0, #0x1
108acf00  1eff2fe1  bx lr
