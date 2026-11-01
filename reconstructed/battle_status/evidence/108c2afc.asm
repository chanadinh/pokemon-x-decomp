108c2afc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2b00  0350a0e1  cpy r5, r3
108c2b04  0240a0e1  cpy r4, r2
108c2b08  0400a0e3  mov r0, #0x4
108c2b0c  82a3feeb  bl 0x1086b91c
108c2b10  040050e1  cmp r0, r4
108c2b14  0400001a  bne 0x108c2b2c
108c2b18  1e00a0e3  mov r0, #0x1e
108c2b1c  7ea3feeb  bl 0x1086b91c
108c2b20  7000ffe6  uxth r0, r0
108c2b24  050050e3  cmp r0, #0x5
108c2b28  0200000a  beq 0x108c2b38
108c2b2c  0000a0e3  mov r0, #0x0
108c2b30  000085e5  str r0, [r5,#0x0]
108c2b34  7080bde8  ldmia sp!,{r4,r5,r6,pc}
108c2b38  0110a0e3  mov r1, #0x1
108c2b3c  4200a0e3  mov r0, #0x42
108c2b40  638bfeeb  bl 0x108658d4
108c2b44  00f020e3  nop
108c2b48  00f020e3  nop
108c2b4c  f7ffffea  b 0x108c2b30
