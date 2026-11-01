108bef24  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bef28  0240a0e1  cpy r4, r2
108bef2c  0350a0e1  cpy r5, r3
108bef30  0200a0e3  mov r0, #0x2
108bef34  78b2feeb  bl 0x1086b91c
108bef38  040050e1  cmp r0, r4
108bef3c  0500001a  bne 0x108bef58
108bef40  2100a0e3  mov r0, #0x21
108bef44  74b2feeb  bl 0x1086b91c
108bef48  001060e2  rsb r1, r0, #0x0
108bef4c  2100a0e3  mov r0, #0x21
108bef50  5f9afeeb  bl 0x108658d4
108bef54  000085e5  str r0, [r5,#0x0]
108bef58  7080bde8  ldmia sp!,{r4,r5,r6,pc}
