108c18d8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c18dc  0240a0e1  cpy r4, r2
108c18e0  0350a0e1  cpy r5, r3
108c18e4  0200a0e3  mov r0, #0x2
108c18e8  0ba8feeb  bl 0x1086b91c
108c18ec  040050e1  cmp r0, r4
108c18f0  0400001a  bne 0x108c1908
108c18f4  0c00a0e3  mov r0, #0xc
108c18f8  07a8feeb  bl 0x1086b91c
108c18fc  070050e3  cmp r0, #0x7
108c1900  0000a003  moveq r0, #0x0
108c1904  00008505  streq r0, [r5,#0x0]
108c1908  7080bde8  ldmia sp!,{r4,r5,r6,pc}
