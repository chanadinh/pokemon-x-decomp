108be2fc  10402de9  stmdb sp!,{r4,lr}
108be300  0240a0e1  cpy r4, r2
108be304  0400a0e3  mov r0, #0x4
108be308  83b5feeb  bl 0x1086b91c
108be30c  040050e1  cmp r0, r4
108be310  0900001a  bne 0x108be33c
108be314  3900a0e3  mov r0, #0x39
108be318  7fb5feeb  bl 0x1086b91c
108be31c  ff0000e2  and r0, r0, #0xff
108be320  9980ffeb  bl 0x1089e58c
108be324  020050e3  cmp r0, #0x2
108be328  0300001a  bne 0x108be33c
108be32c  1040bde8  ldmia sp!,{r4,lr}
108be330  031ba0e3  mov r1, #0xc00
108be334  3600a0e3  mov r0, #0x36
108be338  d13afeea  b 0x1084ce84
108be33c  1080bde8  ldmia sp!,{r4,pc}
