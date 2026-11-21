108c9a64  70002de9  stmdb sp!,{r4,r5,r6}
108c9a68  003090e5  ldr r3, [r0,#0x0]
108c9a6c  50609fe5  ldr r6, [0x108c9ac4]
108c9a70  03c0a0e1  cpy r12, r3
108c9a74  013043e2  sub r3, r3, #0x1
108c9a78  000053e3  cmp r3, #0x0
108c9a7c  0630a0b1  cpylt r3, r6
108c9a80  03005ce1  cmp r12, r3
108c9a84  0b00000a  beq 0x108c9ab8
108c9a88  834180e0  add r4, r0, r3, lsl #0x3
108c9a8c  b850d4e1  ldrh r5, [r4,#0x8]
108c9a90  000055e3  cmp r5, #0x0
108c9a94  0700000a  beq 0x108c9ab8
108c9a98  010055e1  cmp r5, r1
108c9a9c  f4ffff1a  bne 0x108c9a74
108c9aa0  044094e5  ldr r4, [r4,#0x4]
108c9aa4  020054e1  cmp r4, r2
108c9aa8  f1ffff1a  bne 0x108c9a74
108c9aac  7000bde8  ldmia sp!,{r4,r5,r6}
108c9ab0  0100a0e3  mov r0, #0x1
108c9ab4  1eff2fe1  bx lr
108c9ab8  7000bde8  ldmia sp!,{r4,r5,r6}
108c9abc  0000a0e3  mov r0, #0x0
108c9ac0  1eff2fe1  bx lr
