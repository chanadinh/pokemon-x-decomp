108c82f4  10402de9  stmdb sp!,{r4,lr}
108c82f8  1518d0e5  ldrb r1, [r0,#0x815]
108c82fc  021081e2  add r1, r1, #0x2
108c8300  034001e2  and r4, r1, #0x3
108c8304  0410a0e1  cpy r1, r4
108c8308  16a0feeb  bl 0x10870368
108c830c  000050e3  cmp r0, #0x0
108c8310  0400a011  cpyne r0, r4
108c8314  0400a003  moveq r0, #0x4
108c8318  1080bde8  ldmia sp!,{r4,pc}
