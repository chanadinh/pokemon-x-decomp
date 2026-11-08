108a8298  18c093e5  ldr r12, [r3,#0x18]
108a829c  00005ce3  cmp r12, #0x0
108a82a0  1eff2f01  bxeq lr
108a82a4  08402de9  stmdb sp!,{r3,lr}
108a82a8  01c0a0e3  mov r12, #0x1
108a82ac  00c08de5  str r12, [sp,#0x0]
108a82b0  ca90feeb  bl 0x1084c5e0
108a82b4  0880bde8  ldmia sp!,{r3,pc}
