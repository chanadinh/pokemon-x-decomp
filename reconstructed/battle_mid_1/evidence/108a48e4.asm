108a48e4  10402de9  stmdb sp!,{r4,lr}
108a48e8  2c109fe5  ldr r1, [0x108a491c]
108a48ec  0040a0e1  cpy r4, r0
108a48f0  0220a0e3  mov r2, #0x2
108a48f4  5c2dffeb  bl 0x1086fe6c
108a48f8  000050e3  cmp r0, #0x0
108a48fc  0100a013  movne r0, #0x1
108a4900  0400001a  bne 0x108a4918
108a4904  0400a0e1  cpy r0, r4
108a4908  10109fe5  ldr r1, [0x108a4920]
108a490c  1040bde8  ldmia sp!,{r4,lr}
108a4910  0a20a0e3  mov r2, #0xa
108a4914  542dffea  b 0x1086fe6c
108a4918  1080bde8  ldmia sp!,{r4,pc}
