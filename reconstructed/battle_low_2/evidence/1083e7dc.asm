1083e7dc  14109fe5  ldr r1, [0x1083e7f8]
1083e7e0  14009fe5  ldr r0, [0x1083e7fc]
1083e7e4  6c2091e5  ldr r2, [r1,#0x6c]
1083e7e8  000052e1  cmp r2, r0
1083e7ec  6c008115  strne r0, [r1,#0x6c]
1083e7f0  0100a0e3  mov r0, #0x1
1083e7f4  1eff2fe1  bx lr
