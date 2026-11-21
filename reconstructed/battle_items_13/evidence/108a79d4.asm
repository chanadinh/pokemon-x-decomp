108a79d4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a79d8  0150a0e1  cpy r5, r1
108a79dc  0240a0e1  cpy r4, r2
108a79e0  0200a0e3  mov r0, #0x2
108a79e4  cc0fffeb  bl 0x1086b91c
108a79e8  040050e1  cmp r0, r4
108a79ec  0400001a  bne 0x108a7a04
108a79f0  0410a0e1  cpy r1, r4
108a79f4  0500a0e1  cpy r0, r5
108a79f8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a79fc  0020a0e3  mov r2, #0x0
108a7a00  7192feea  b 0x1084c3cc
108a7a04  7080bde8  ldmia sp!,{r4,r5,r6,pc}
