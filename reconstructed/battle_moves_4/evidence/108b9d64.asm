108b9d64  10402de9  stmdb sp!,{r4,lr}
108b9d68  0240a0e1  cpy r4, r2
108b9d6c  0300a0e3  mov r0, #0x3
108b9d70  e9c6feeb  bl 0x1086b91c
108b9d74  040050e1  cmp r0, r4
108b9d78  0300001a  bne 0x108b9d8c
108b9d7c  1040bde8  ldmia sp!,{r4,lr}
108b9d80  0010a0e3  mov r1, #0x0
108b9d84  4300a0e3  mov r0, #0x43
108b9d88  d1aefeea  b 0x108658d4
108b9d8c  1080bde8  ldmia sp!,{r4,pc}
