108bd7cc  10402de9  stmdb sp!,{r4,lr}
108bd7d0  0240a0e1  cpy r4, r2
108bd7d4  0300a0e3  mov r0, #0x3
108bd7d8  4fb8feeb  bl 0x1086b91c
108bd7dc  040050e1  cmp r0, r4
108bd7e0  0300001a  bne 0x108bd7f4
108bd7e4  0c109fe5  ldr r1, [0x108bd7f8]
108bd7e8  1040bde8  ldmia sp!,{r4,lr}
108bd7ec  3600a0e3  mov r0, #0x36
108bd7f0  a33dfeea  b 0x1084ce84
108bd7f4  1080bde8  ldmia sp!,{r4,pc}
