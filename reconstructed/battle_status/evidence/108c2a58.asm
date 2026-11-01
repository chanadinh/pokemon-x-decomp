108c2a58  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2a5c  0350a0e1  cpy r5, r3
108c2a60  0240a0e1  cpy r4, r2
108c2a64  0400a0e3  mov r0, #0x4
108c2a68  aba3feeb  bl 0x1086b91c
108c2a6c  040050e1  cmp r0, r4
108c2a70  0400001a  bne 0x108c2a88
108c2a74  1e00a0e3  mov r0, #0x1e
108c2a78  a7a3feeb  bl 0x1086b91c
108c2a7c  7000ffe6  uxth r0, r0
108c2a80  010050e3  cmp r0, #0x1
108c2a84  0200000a  beq 0x108c2a94
108c2a88  0000a0e3  mov r0, #0x0
108c2a8c  000085e5  str r0, [r5,#0x0]
108c2a90  7080bde8  ldmia sp!,{r4,r5,r6,pc}
108c2a94  0110a0e3  mov r1, #0x1
108c2a98  4200a0e3  mov r0, #0x42
108c2a9c  8c8bfeeb  bl 0x108658d4
108c2aa0  00f020e3  nop
108c2aa4  00f020e3  nop
108c2aa8  f7ffffea  b 0x108c2a8c
