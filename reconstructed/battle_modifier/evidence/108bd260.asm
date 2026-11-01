108bd260  10402de9  stmdb sp!,{r4,lr}
108bd264  0240a0e1  cpy r4, r2
108bd268  0300a0e3  mov r0, #0x3
108bd26c  aab9feeb  bl 0x1086b91c
108bd270  040050e1  cmp r0, r4
108bd274  0700001a  bne 0x108bd298
108bd278  4600a0e3  mov r0, #0x46
108bd27c  a6b9feeb  bl 0x1086b91c
108bd280  000050e3  cmp r0, #0x0
108bd284  0300000a  beq 0x108bd298
108bd288  1040bde8  ldmia sp!,{r4,lr}
108bd28c  061ba0e3  mov r1, #0x1800
108bd290  3600a0e3  mov r0, #0x36
108bd294  fa3efeea  b 0x1084ce84
108bd298  1080bde8  ldmia sp!,{r4,pc}
