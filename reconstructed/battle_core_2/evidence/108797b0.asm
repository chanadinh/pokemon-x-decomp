108797b0  7e3fa0e3  mov r3, #0x1f8
108797b4  7e2ca0e3  mov r2, #0x7e00
108797b8  811103e0  and r1, r3, r1, lsl #0x3
108797bc  041081e3  orr r1, r1, #0x4
108797c0  800402e0  and r0, r2, r0, lsl #0x9
108797c4  010080e1  orr r0, r0, r1
108797c8  1eff2fe1  bx lr
