10889aac  38402de9  stmdb sp!,{r3,r4,r5,lr}
10889ab0  0140a0e1  cpy r4, r1
10889ab4  0120a0e3  mov r2, #0x1
10889ab8  0d10a0e1  cpy r1, sp
10889abc  90dafdeb  bl 0x10800504
10889ac0  0400a0e1  cpy r0, r4
10889ac4  57c4feeb  bl 0x1083ac28
10889ac8  3880bde8  ldmia sp!,{r3,r4,r5,pc}
