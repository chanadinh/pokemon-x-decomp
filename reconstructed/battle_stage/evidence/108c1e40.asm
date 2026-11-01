108c1e40  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c1e44  0350a0e1  cpy r5, r3
108c1e48  0240a0e1  cpy r4, r2
108c1e4c  0200a0e3  mov r0, #0x2
108c1e50  b1a6feeb  bl 0x1086b91c
108c1e54  040050e1  cmp r0, r4
108c1e58  0f00001a  bne 0x108c1e9c
108c1e5c  0300a0e3  mov r0, #0x3
108c1e60  ada6feeb  bl 0x1086b91c
108c1e64  040050e1  cmp r0, r4
108c1e68  0b00000a  beq 0x108c1e9c
108c1e6c  2000a0e3  mov r0, #0x20
108c1e70  a9a6feeb  bl 0x1086b91c
108c1e74  020050e3  cmp r0, #0x2
108c1e78  0700001a  bne 0x108c1e9c
108c1e7c  2100a0e3  mov r0, #0x21
108c1e80  a5a6feeb  bl 0x1086b91c
108c1e84  000050e3  cmp r0, #0x0
108c1e88  030000aa  bge 0x108c1e9c
108c1e8c  0110a0e3  mov r1, #0x1
108c1e90  4200a0e3  mov r0, #0x42
108c1e94  8e8efeeb  bl 0x108658d4
108c1e98  000085e5  str r0, [r5,#0x0]
108c1e9c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
