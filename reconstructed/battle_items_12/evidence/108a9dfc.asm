108a9dfc  18c093e5  ldr r12, [r3,#0x18]
108a9e00  00005ce3  cmp r12, #0x0
108a9e04  1eff2f01  bxeq lr
108a9e08  08402de9  stmdb sp!,{r3,lr}
108a9e0c  01c0a0e3  mov r12, #0x1
108a9e10  00c08de5  str r12, [sp,#0x0]
108a9e14  000000eb  bl 0x108a9e1c
108a9e18  0880bde8  ldmia sp!,{r3,pc}
