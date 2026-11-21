1083bad8  34209fe5  ldr r2, [0x1083bb14]
1083badc  10402de9  stmdb sp!,{r4,lr}
1083bae0  30109fe5  ldr r1, [0x1083bb18]
1083bae4  6c3092e5  ldr r3, [r2,#0x6c]
1083bae8  010053e1  cmp r3, r1
1083baec  6c108215  strne r1, [r2,#0x6c]
1083baf0  904190e5  ldr r4, [r0,#0x190]
1083baf4  0310a0e3  mov r1, #0x3
1083baf8  5c0694e5  ldr r0, [r4,#0x65c]
1083bafc  5813ffeb  bl 0x10800864
1083bb00  5c0694e5  ldr r0, [r4,#0x65c]
1083bb04  1040bde8  ldmia sp!,{r4,lr}
1083bb08  0210a0e3  mov r1, #0x2
1083bb0c  0120a0e3  mov r2, #0x1
1083bb10  5713ffea  b 0x10800874
