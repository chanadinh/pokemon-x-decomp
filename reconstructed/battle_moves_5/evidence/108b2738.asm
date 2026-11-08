108b2738  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b273c  0150a0e1  cpy r5, r1
108b2740  0240a0e1  cpy r4, r2
108b2744  0300a0e3  mov r0, #0x3
108b2748  73e4feeb  bl 0x1086b91c
108b274c  040050e1  cmp r0, r4
108b2750  1e00001a  bne 0x108b27d0
108b2754  0710a0e3  mov r1, #0x7
108b2758  0500a0e1  cpy r0, r5
108b275c  0467feeb  bl 0x1084c374
108b2760  000050e3  cmp r0, #0x0
108b2764  0760a0e3  mov r6, #0x7
108b2768  0420a0e1  cpy r2, r4
108b276c  0700000a  beq 0x108b2790
108b2770  1d10a0e3  mov r1, #0x1d
108b2774  0500a0e1  cpy r0, r5
108b2778  ffddfeeb  bl 0x10869f7c
108b277c  0010a0e1  cpy r1, r0
108b2780  0460c0e5  strb r6, [r0,#0x4]
108b2784  0500a0e1  cpy r0, r5
108b2788  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b278c  edd4feea  b 0x10867b48
108b2790  1c10a0e3  mov r1, #0x1c
108b2794  0500a0e1  cpy r0, r5
108b2798  f7ddfeeb  bl 0x10869f7c
108b279c  0040a0e1  cpy r4, r0
108b27a0  0460c0e5  strb r6, [r0,#0x4]
108b27a4  0500a0e3  mov r0, #0x5
108b27a8  a365feeb  bl 0x1084be3c
108b27ac  080084e5  str r0, [r4,#0x8]
108b27b0  ba20a0e3  mov r2, #0xba
108b27b4  0110a0e3  mov r1, #0x1
108b27b8  0c0084e2  add r0, r4, #0xc
108b27bc  c46afeeb  bl 0x1084d2d4
108b27c0  0410a0e1  cpy r1, r4
108b27c4  0500a0e1  cpy r0, r5
108b27c8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b27cc  ddd4feea  b 0x10867b48
108b27d0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
