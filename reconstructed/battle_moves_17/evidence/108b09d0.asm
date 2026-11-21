108b09d0  38402de9  stmdb sp!,{r3,r4,r5,lr}
108b09d4  0240a0e1  cpy r4, r2
108b09d8  0300a0e3  mov r0, #0x3
108b09dc  ceebfeeb  bl 0x1086b91c
108b09e0  040050e1  cmp r0, r4
108b09e4  0800001a  bne 0x108b0a0c
108b09e8  1f00a0e3  mov r0, #0x1f
108b09ec  caebfeeb  bl 0x1086b91c
108b09f0  00008de5  str r0, [sp,#0x0]
108b09f4  0710a0e3  mov r1, #0x7
108b09f8  0d00a0e1  cpy r0, sp
108b09fc  e36bfeeb  bl 0x1084b990
108b0a00  00109de5  ldr r1, [sp,#0x0]
108b0a04  1f00a0e3  mov r0, #0x1f
108b0a08  b1d3feeb  bl 0x108658d4
108b0a0c  3880bde8  ldmia sp!,{r3,r4,r5,pc}
