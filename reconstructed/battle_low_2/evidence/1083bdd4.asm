1083bdd4  24209fe5  ldr r2, [0x1083be00]
1083bdd8  10402de9  stmdb sp!,{r4,lr}
1083bddc  20109fe5  ldr r1, [0x1083be04]
1083bde0  6c3092e5  ldr r3, [r2,#0x6c]
1083bde4  010053e1  cmp r3, r1
1083bde8  6c108215  strne r1, [r2,#0x6c]
1083bdec  900190e5  ldr r0, [r0,#0x190]
1083bdf0  0a0d80e2  add r0, r0, #0x280
1083bdf4  b27700eb  bl 0x10859cc4
1083bdf8  0100a0e3  mov r0, #0x1
1083bdfc  1080bde8  ldmia sp!,{r4,pc}
