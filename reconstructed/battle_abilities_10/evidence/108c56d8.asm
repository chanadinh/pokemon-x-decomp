108c56d8  f8432de9  stmdb sp!,{r3,r4,r5,r6,r7,r8,r9,lr}
108c56dc  0140a0e1  cpy r4, r1
108c56e0  0070a0e1  cpy r7, r0
108c56e4  0100a0e1  cpy r0, r1
108c56e8  0360a0e1  cpy r6, r3
108c56ec  0250a0e1  cpy r5, r2
108c56f0  0210a0e1  cpy r1, r2
108c56f4  693bfeeb  bl 0x108544a0
108c56f8  bc00d0e1  ldrh r0, [r0,#0xc]
108c56fc  011c40e2  sub r1, r0, #0x100
108c5700  a51051e2  subs r1, r1, #0xa5
108c5704  0180a003  moveq r8, #0x1
108c5708  0a00001a  bne 0x108c5738
108c570c  0400a0e1  cpy r0, r4
108c5710  d019feeb  bl 0x1084be58
108c5714  010050e3  cmp r0, #0x1
108c5718  0130a003  moveq r3, #0x1
108c571c  0030a013  movne r3, #0x0
108c5720  0520a0e1  cpy r2, r5
108c5724  0410a0e1  cpy r1, r4
108c5728  0700a0e1  cpy r0, r7
108c572c  00808de5  str r8, [sp,#0x0]
108c5730  8314feeb  bl 0x1084a944
108c5734  008086e5  str r8, [r6,#0x0]
108c5738  f883bde8  ldmia sp!,{r3,r4,r5,r6,r7,r8,r9,pc}
