108b2e8c  10402de9  stmdb sp!,{r4,lr}
108b2e90  0240a0e1  cpy r4, r2
108b2e94  0300a0e3  mov r0, #0x3
108b2e98  9fe2feeb  bl 0x1086b91c
108b2e9c  040050e1  cmp r0, r4
108b2ea0  0300001a  bne 0x108b2eb4
108b2ea4  1040bde8  ldmia sp!,{r4,lr}
108b2ea8  1410a0e3  mov r1, #0x14
108b2eac  3800a0e3  mov r0, #0x38
108b2eb0  87cafeea  b 0x108658d4
108b2eb4  1080bde8  ldmia sp!,{r4,pc}
