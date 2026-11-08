108be2c0  10402de9  stmdb sp!,{r4,lr}
108be2c4  0240a0e1  cpy r4, r2
108be2c8  0300a0e3  mov r0, #0x3
108be2cc  92b5feeb  bl 0x1086b91c
108be2d0  ff1000e2  and r1, r0, #0xff
108be2d4  0400a0e1  cpy r0, r4
108be2d8  b238feeb  bl 0x1084c5a8
108be2dc  000050e3  cmp r0, #0x0
108be2e0  0300000a  beq 0x108be2f4
108be2e4  0c109fe5  ldr r1, [0x108be2f8]
108be2e8  1040bde8  ldmia sp!,{r4,lr}
108be2ec  3600a0e3  mov r0, #0x36
108be2f0  e33afeea  b 0x1084ce84
108be2f4  1080bde8  ldmia sp!,{r4,pc}
