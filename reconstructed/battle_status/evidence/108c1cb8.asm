108c1cb8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c1cbc  0240a0e1  cpy r4, r2
108c1cc0  0350a0e1  cpy r5, r3
108c1cc4  0400a0e3  mov r0, #0x4
108c1cc8  13a7feeb  bl 0x1086b91c
108c1ccc  040050e1  cmp r0, r4
108c1cd0  0400001a  bne 0x108c1ce8
108c1cd4  1e00a0e3  mov r0, #0x1e
108c1cd8  0fa7feeb  bl 0x1086b91c
108c1cdc  7000ffe6  uxth r0, r0
108c1ce0  020050e3  cmp r0, #0x2
108c1ce4  1000000a  beq 0x108c1d2c
108c1ce8  0000a0e3  mov r0, #0x0
108c1cec  000050e3  cmp r0, #0x0
108c1cf0  000085e5  str r0, [r5,#0x0]
108c1cf4  0b00001a  bne 0x108c1d28
108c1cf8  0400a0e3  mov r0, #0x4
108c1cfc  06a7feeb  bl 0x1086b91c
108c1d00  040050e1  cmp r0, r4
108c1d04  00f020e3  nop
108c1d08  0400001a  bne 0x108c1d20
108c1d0c  1e00a0e3  mov r0, #0x1e
108c1d10  01a7feeb  bl 0x1086b91c
108c1d14  7000ffe6  uxth r0, r0
108c1d18  0e0050e3  cmp r0, #0xe
108c1d1c  0800000a  beq 0x108c1d44
108c1d20  0000a0e3  mov r0, #0x0
108c1d24  000085e5  str r0, [r5,#0x0]
108c1d28  7080bde8  ldmia sp!,{r4,r5,r6,pc}
108c1d2c  0110a0e3  mov r1, #0x1
108c1d30  4200a0e3  mov r0, #0x42
108c1d34  e68efeeb  bl 0x108658d4
108c1d38  00f020e3  nop
108c1d3c  00f020e3  nop
108c1d40  e9ffffea  b 0x108c1cec
108c1d44  0110a0e3  mov r1, #0x1
108c1d48  4200a0e3  mov r0, #0x42
108c1d4c  e08efeeb  bl 0x108658d4
108c1d50  00f020e3  nop
108c1d54  00f020e3  nop
108c1d58  f1ffffea  b 0x108c1d24
