108bd29c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd2a0  0150a0e1  cpy r5, r1
108bd2a4  0240a0e1  cpy r4, r2
108bd2a8  0200a0e3  mov r0, #0x2
108bd2ac  9ab9feeb  bl 0x1086b91c
108bd2b0  040050e1  cmp r0, r4
108bd2b4  0700001a  bne 0x108bd2d8
108bd2b8  0500a0e1  cpy r0, r5
108bd2bc  e53afeeb  bl 0x1084be58
108bd2c0  020050e3  cmp r0, #0x2
108bd2c4  0300001a  bne 0x108bd2d8
108bd2c8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd2cc  021aa0e3  mov r1, #0x2000
108bd2d0  3600a0e3  mov r0, #0x36
108bd2d4  ea3efeea  b 0x1084ce84
108bd2d8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
