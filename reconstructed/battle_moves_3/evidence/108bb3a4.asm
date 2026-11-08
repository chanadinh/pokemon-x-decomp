108bb3a4  0100a0e1  cpy r0, r1
108bb3a8  02c0a0e1  cpy r12, r2
108bb3ac  08402de9  stmdb sp!,{r3,lr}
108bb3b0  061ba0e3  mov r1, #0x1800
108bb3b4  0320a0e1  cpy r2, r3
108bb3b8  00108de5  str r1, [sp,#0x0]
108bb3bc  0030a0e3  mov r3, #0x0
108bb3c0  0c10a0e1  cpy r1, r12
108bb3c4  173ffeeb  bl 0x1084b028
108bb3c8  0880bde8  ldmia sp!,{r3,pc}
