108bd6b4  10402de9  stmdb sp!,{r4,lr}
108bd6b8  0240a0e1  cpy r4, r2
108bd6bc  0200a0e3  mov r0, #0x2
108bd6c0  95b8feeb  bl 0x1086b91c
108bd6c4  040050e1  cmp r0, r4
108bd6c8  0300001a  bne 0x108bd6dc
108bd6cc  1040bde8  ldmia sp!,{r4,lr}
108bd6d0  0010a0e3  mov r1, #0x0
108bd6d4  1100a0e3  mov r0, #0x11
108bd6d8  7da0feea  b 0x108658d4
108bd6dc  1080bde8  ldmia sp!,{r4,pc}
