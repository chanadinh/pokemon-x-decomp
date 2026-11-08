108aa524  10402de9  stmdb sp!,{r4,lr}
108aa528  0240a0e1  cpy r4, r2
108aa52c  0200a0e3  mov r0, #0x2
108aa530  f904ffeb  bl 0x1086b91c
108aa534  040050e1  cmp r0, r4
108aa538  0300001a  bne 0x108aa54c
108aa53c  1040bde8  ldmia sp!,{r4,lr}
108aa540  0110a0e3  mov r1, #0x1
108aa544  5300a0e3  mov r0, #0x53
108aa548  e1ecfeea  b 0x108658d4
108aa54c  1080bde8  ldmia sp!,{r4,pc}
