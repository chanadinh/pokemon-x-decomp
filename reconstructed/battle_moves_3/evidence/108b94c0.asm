108b94c0  0100a0e1  cpy r0, r1
108b94c4  02c0a0e1  cpy r12, r2
108b94c8  08402de9  stmdb sp!,{r3,lr}
108b94cc  021aa0e3  mov r1, #0x2000
108b94d0  0320a0e1  cpy r2, r3
108b94d4  00108de5  str r1, [sp,#0x0]
108b94d8  0130a0e3  mov r3, #0x1
108b94dc  0c10a0e1  cpy r1, r12
108b94e0  d046feeb  bl 0x1084b028
108b94e4  0880bde8  ldmia sp!,{r3,pc}
