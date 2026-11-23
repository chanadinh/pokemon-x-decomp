108c9e84  ff1001e2  and r1, r1, #0xff
108c9e88  011481e1  orr r1, r1, r1, lsl #0x8
108c9e8c  011881e1  orr r1, r1, r1, lsl #0x10
108c9e90  001080e5  str r1, [r0,#0x0]
108c9e94  041080e5  str r1, [r0,#0x4]
108c9e98  b810c0e1  strh r1, [r0,#0x8]
108c9e9c  1eff2fe1  bx lr
