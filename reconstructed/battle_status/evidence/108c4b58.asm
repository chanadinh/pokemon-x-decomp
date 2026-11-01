108c4b58  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c4b5c  0350a0e1  cpy r5, r3
108c4b60  0240a0e1  cpy r4, r2
108c4b64  0400a0e3  mov r0, #0x4
108c4b68  6b9bfeeb  bl 0x1086b91c
108c4b6c  040050e1  cmp r0, r4
108c4b70  0400001a  bne 0x108c4b88
108c4b74  1e00a0e3  mov r0, #0x1e
108c4b78  679bfeeb  bl 0x1086b91c
108c4b7c  7000ffe6  uxth r0, r0
108c4b80  040050e3  cmp r0, #0x4
108c4b84  0200000a  beq 0x108c4b94
108c4b88  0000a0e3  mov r0, #0x0
108c4b8c  000085e5  str r0, [r5,#0x0]
108c4b90  7080bde8  ldmia sp!,{r4,r5,r6,pc}
108c4b94  0110a0e3  mov r1, #0x1
108c4b98  4200a0e3  mov r0, #0x42
108c4b9c  4c83feeb  bl 0x108658d4
108c4ba0  00f020e3  nop
108c4ba4  00f020e3  nop
108c4ba8  f7ffffea  b 0x108c4b8c
