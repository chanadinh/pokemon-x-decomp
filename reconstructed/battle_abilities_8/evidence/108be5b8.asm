108be5b8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108be5bc  0240a0e1  cpy r4, r2
108be5c0  0350a0e1  cpy r5, r3
108be5c4  0200a0e3  mov r0, #0x2
108be5c8  d3b4feeb  bl 0x1086b91c
108be5cc  040050e1  cmp r0, r4
108be5d0  0700001a  bne 0x108be5f4
108be5d4  2e00a0e3  mov r0, #0x2e
108be5d8  cfb4feeb  bl 0x1086b91c
108be5dc  000050e3  cmp r0, #0x0
108be5e0  0300001a  bne 0x108be5f4
108be5e4  0110a0e3  mov r1, #0x1
108be5e8  4200a0e3  mov r0, #0x42
108be5ec  b89cfeeb  bl 0x108658d4
108be5f0  000085e5  str r0, [r5,#0x0]
108be5f4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
