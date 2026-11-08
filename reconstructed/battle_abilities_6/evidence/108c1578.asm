108c1578  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c157c  0140a0e1  cpy r4, r1
108c1580  0250a0e1  cpy r5, r2
108c1584  0720a0e3  mov r2, #0x7
108c1588  0510a0e1  cpy r1, r5
108c158c  0400a0e1  cpy r0, r4
108c1590  8325feeb  bl 0x1084aba4
108c1594  0510a0e1  cpy r1, r5
108c1598  0400a0e1  cpy r0, r4
108c159c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c15a0  0b20a0e3  mov r2, #0xb
108c15a4  7e25feea  b 0x1084aba4
