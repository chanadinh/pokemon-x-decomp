108c2b88  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2b8c  0350a0e1  cpy r5, r3
108c2b90  0240a0e1  cpy r4, r2
108c2b94  0400a0e3  mov r0, #0x4
108c2b98  5fa3feeb  bl 0x1086b91c
108c2b9c  040050e1  cmp r0, r4
108c2ba0  0400001a  bne 0x108c2bb8
108c2ba4  1e00a0e3  mov r0, #0x1e
108c2ba8  5ba3feeb  bl 0x1086b91c
108c2bac  7000ffe6  uxth r0, r0
108c2bb0  060050e3  cmp r0, #0x6
108c2bb4  0200000a  beq 0x108c2bc4
108c2bb8  0000a0e3  mov r0, #0x0
108c2bbc  000085e5  str r0, [r5,#0x0]
108c2bc0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
108c2bc4  0110a0e3  mov r1, #0x1
108c2bc8  4200a0e3  mov r0, #0x42
108c2bcc  408bfeeb  bl 0x108658d4
108c2bd0  00f020e3  nop
108c2bd4  00f020e3  nop
108c2bd8  f7ffffea  b 0x108c2bbc
