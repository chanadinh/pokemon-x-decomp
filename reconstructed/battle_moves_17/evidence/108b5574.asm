108b5574  10402de9  stmdb sp!,{r4,lr}
108b5578  0240a0e1  cpy r4, r2
108b557c  0300a0e3  mov r0, #0x3
108b5580  e5d8feeb  bl 0x1086b91c
108b5584  040050e1  cmp r0, r4
108b5588  0f00001a  bne 0x108b55cc
108b558c  4000a0e3  mov r0, #0x40
108b5590  e1d8feeb  bl 0x1086b91c
108b5594  0040a0e1  cpy r4, r0
108b5598  2010a0e3  mov r1, #0x20
108b559c  1e00a0e3  mov r0, #0x1e
108b55a0  cbc0feeb  bl 0x108658d4
108b55a4  24209fe5  ldr r2, [0x108b55d0]
108b55a8  0210a0e3  mov r1, #0x2
108b55ac  0400a0e1  cpy r0, r4
108b55b0  475ffeeb  bl 0x1084d2d4
108b55b4  0400a0e3  mov r0, #0x4
108b55b8  d7d8feeb  bl 0x1086b91c
108b55bc  0010a0e1  cpy r1, r0
108b55c0  0400a0e1  cpy r0, r4
108b55c4  1040bde8  ldmia sp!,{r4,lr}
108b55c8  335ffeea  b 0x1084d29c
108b55cc  1080bde8  ldmia sp!,{r4,pc}
