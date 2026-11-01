108c2bdc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2be0  0350a0e1  cpy r5, r3
108c2be4  0240a0e1  cpy r4, r2
108c2be8  0200a0e3  mov r0, #0x2
108c2bec  4aa3feeb  bl 0x1086b91c
108c2bf0  040050e1  cmp r0, r4
108c2bf4  0f00001a  bne 0x108c2c38
108c2bf8  0300a0e3  mov r0, #0x3
108c2bfc  46a3feeb  bl 0x1086b91c
108c2c00  040050e1  cmp r0, r4
108c2c04  0b00000a  beq 0x108c2c38
108c2c08  2000a0e3  mov r0, #0x20
108c2c0c  42a3feeb  bl 0x1086b91c
108c2c10  060050e3  cmp r0, #0x6
108c2c14  0700001a  bne 0x108c2c38
108c2c18  2100a0e3  mov r0, #0x21
108c2c1c  3ea3feeb  bl 0x1086b91c
108c2c20  000050e3  cmp r0, #0x0
108c2c24  030000aa  bge 0x108c2c38
108c2c28  0110a0e3  mov r1, #0x1
108c2c2c  4200a0e3  mov r0, #0x42
108c2c30  278bfeeb  bl 0x108658d4
108c2c34  000085e5  str r0, [r5,#0x0]
108c2c38  7080bde8  ldmia sp!,{r4,r5,r6,pc}
