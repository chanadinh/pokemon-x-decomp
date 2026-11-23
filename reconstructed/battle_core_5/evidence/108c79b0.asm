108c79b0  811161e0  rsb r1, r1, r1, lsl #0x3
108c79b4  810180e0  add r0, r0, r1, lsl #0x3
108c79b8  381890e5  ldr r1, [r0,#0x838]
108c79bc  000051e3  cmp r1, #0x0
108c79c0  020b8002  addeq r0, r0, #0x800
108c79c4  bc04d001  ldrheq r0, [r0,#0x4c]
108c79c8  0000a013  movne r0, #0x0
108c79cc  1eff2fe1  bx lr
