108bbd34  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bbd38  0150a0e1  cpy r5, r1
108bbd3c  0240a0e1  cpy r4, r2
108bbd40  0300a0e3  mov r0, #0x3
108bbd44  f4befeeb  bl 0x1086b91c
108bbd48  040050e1  cmp r0, r4
108bbd4c  0800001a  bne 0x108bbd74
108bbd50  0420a0e1  cpy r2, r4
108bbd54  1210a0e3  mov r1, #0x12
108bbd58  0500a0e1  cpy r0, r5
108bbd5c  86b8feeb  bl 0x10869f7c
108bbd60  0010a0e1  cpy r1, r0
108bbd64  0440c0e5  strb r4, [r0,#0x4]
108bbd68  0500a0e1  cpy r0, r5
108bbd6c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bbd70  74affeea  b 0x10867b48
108bbd74  7080bde8  ldmia sp!,{r4,r5,r6,pc}
