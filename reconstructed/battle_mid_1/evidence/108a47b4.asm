108a47b4  10402de9  stmdb sp!,{r4,lr}
108a47b8  2c109fe5  ldr r1, [0x108a47ec]
108a47bc  0040a0e1  cpy r4, r0
108a47c0  0e20a0e3  mov r2, #0xe
108a47c4  a82dffeb  bl 0x1086fe6c
108a47c8  000050e3  cmp r0, #0x0
108a47cc  0100a013  movne r0, #0x1
108a47d0  0400001a  bne 0x108a47e8
108a47d4  0400a0e1  cpy r0, r4
108a47d8  10109fe5  ldr r1, [0x108a47f0]
108a47dc  1040bde8  ldmia sp!,{r4,lr}
108a47e0  2020a0e3  mov r2, #0x20
108a47e4  a02dffea  b 0x1086fe6c
108a47e8  1080bde8  ldmia sp!,{r4,pc}
