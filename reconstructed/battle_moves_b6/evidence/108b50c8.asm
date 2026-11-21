108b50c8  0100a0e1  cpy r0, r1
108b50cc  10402de9  stmdb sp!,{r4,lr}
108b50d0  0210a0e1  cpy r1, r2
108b50d4  0340a0e1  cpy r4, r3
108b50d8  1a5bfeeb  bl 0x1084bd48
108b50dc  0100a0e3  mov r0, #0x1
108b50e0  180084e5  str r0, [r4,#0x18]
108b50e4  1080bde8  ldmia sp!,{r4,pc}
