108704ac  70c0a0e3  mov r12, #0x70
108704b0  0e3da0e3  mov r3, #0x380
108704b4  01120ce0  and r1, r12, r1, lsl #0x4
108704b8  031081e3  orr r1, r1, #0x3
108704bc  822303e0  and r2, r3, r2, lsl #0x7
108704c0  021081e1  orr r1, r1, r2
108704c4  001080e5  str r1, [r0,#0x0]
108704c8  1eff2fe1  bx lr
