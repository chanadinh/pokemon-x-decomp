1081cf14  10402de9  stmdb sp!,{r4,lr}
1081cf18  10d04de2  sub sp, sp, #0x10
1081cf1c  18e08de2  add lr, sp, #0x18
1081cf20  0240a0e1  cpy r4, r2
1081cf24  04109ee8  ldmia lr,{r2,r12}
1081cf28  04e08de2  add lr, sp, #0x4
1081cf2c  0c0090e5  ldr r0, [r0,#0xc]
1081cf30  00308de5  str r3, [sp,#0x0]
1081cf34  04108ee8  stmia lr,{r2,r12}
1081cf38  0120a0e1  cpy r2, r1
1081cf3c  0430a0e1  cpy r3, r4
1081cf40  3710a0e3  mov r1, #0x37
1081cf44  b83c01eb  bl 0x1086c22c
1081cf48  10d08de2  add sp, sp, #0x10
1081cf4c  1080bde8  ldmia sp!,{r4,pc}
