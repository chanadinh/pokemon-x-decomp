108b0980  38402de9  stmdb sp!,{r3,r4,r5,lr}
108b0984  0050a0e1  cpy r5, r0
108b0988  0240a0e1  cpy r4, r2
108b098c  0300a0e3  mov r0, #0x3
108b0990  e1ebfeeb  bl 0x1086b91c
108b0994  040050e1  cmp r0, r4
108b0998  0b00001a  bne 0x108b09cc
108b099c  0500a0e1  cpy r0, r5
108b09a0  b803d0e1  ldrh r0, [r0,#0x38]
108b09a4  0040a0e1  cpy r4, r0
108b09a8  1f00a0e3  mov r0, #0x1f
108b09ac  daebfeeb  bl 0x1086b91c
108b09b0  00008de5  str r0, [sp,#0x0]
108b09b4  0410a0e1  cpy r1, r4
108b09b8  0d00a0e1  cpy r0, sp
108b09bc  f36bfeeb  bl 0x1084b990
108b09c0  00109de5  ldr r1, [sp,#0x0]
108b09c4  1f00a0e3  mov r0, #0x1f
108b09c8  c1d3feeb  bl 0x108658d4
108b09cc  3880bde8  ldmia sp!,{r3,r4,r5,pc}
