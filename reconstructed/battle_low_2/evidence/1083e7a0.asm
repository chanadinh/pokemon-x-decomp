1083e7a0  2c209fe5  ldr r2, [0x1083e7d4]
1083e7a4  10402de9  stmdb sp!,{r4,lr}
1083e7a8  28109fe5  ldr r1, [0x1083e7d8]
1083e7ac  6c3092e5  ldr r3, [r2,#0x6c]
1083e7b0  010053e1  cmp r3, r1
1083e7b4  6c108215  strne r1, [r2,#0x6c]
1083e7b8  900190e5  ldr r0, [r0,#0x190]
1083e7bc  0010a0e3  mov r1, #0x0
1083e7c0  f40490e5  ldr r0, [r0,#0x4f4]
1083e7c4  320affeb  bl 0x10801094
1083e7c8  000050e3  cmp r0, #0x0
1083e7cc  0100a013  movne r0, #0x1
1083e7d0  1080bde8  ldmia sp!,{r4,pc}
