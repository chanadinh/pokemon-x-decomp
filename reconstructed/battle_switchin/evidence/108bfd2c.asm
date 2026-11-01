108bfd2c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bfd30  0260a0e3  mov r6, #0x2
108bfd34  0150a0e1  cpy r5, r1
108bfd38  0240a0e1  cpy r4, r2
108bfd3c  0600a0e1  cpy r0, r6
108bfd40  f5aefeeb  bl 0x1086b91c
108bfd44  040050e1  cmp r0, r4
108bfd48  0400001a  bne 0x108bfd60
108bfd4c  0620a0e1  cpy r2, r6
108bfd50  0410a0e1  cpy r1, r4
108bfd54  0500a0e1  cpy r0, r5
108bfd58  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bfd5c  902bfeea  b 0x1084aba4
108bfd60  7080bde8  ldmia sp!,{r4,r5,r6,pc}
