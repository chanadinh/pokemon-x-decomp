108be340  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108be344  0250a0e1  cpy r5, r2
108be348  0170a0e1  cpy r7, r1
108be34c  0500a0e3  mov r0, #0x5
108be350  71b5feeb  bl 0x1086b91c
108be354  006050e2  subs r6, r0, #0x0
108be358  0040a0e3  mov r4, #0x0
108be35c  3600009a  bls 0x108be43c
108be360  060084e2  add r0, r4, #0x6
108be364  ff0000e2  and r0, r0, #0xff
108be368  6bb5feeb  bl 0x1086b91c
108be36c  050050e1  cmp r0, r5
108be370  00f020e3  nop
108be374  2d00001a  bne 0x108be430
108be378  0700a0e1  cpy r0, r7
108be37c  319efeeb  bl 0x10865c48
108be380  000050e3  cmp r0, #0x0
108be384  00f020e3  nop
108be388  2b00001a  bne 0x108be43c
108be38c  0510a0e1  cpy r1, r5
108be390  0700a0e1  cpy r0, r7
108be394  4158feeb  bl 0x108544a0
108be398  0060a0e1  cpy r6, r0
108be39c  00f020e3  nop
108be3a0  2785feeb  bl 0x1085f844
108be3a4  000050e3  cmp r0, #0x0
108be3a8  00f020e3  nop
108be3ac  2200001a  bne 0x108be43c
108be3b0  1600a0e3  mov r0, #0x16
108be3b4  58b5feeb  bl 0x1086b91c
108be3b8  ff4000e2  and r4, r0, #0xff
108be3bc  120054e3  cmp r4, #0x12
108be3c0  1d00000a  beq 0x108be43c
108be3c4  0410a0e1  cpy r1, r4
108be3c8  0600a0e1  cpy r0, r6
108be3cc  63c5feeb  bl 0x1086f960
108be3d0  000050e3  cmp r0, #0x0
108be3d4  00f020e3  nop
108be3d8  1700001a  bne 0x108be43c
108be3dc  0520a0e1  cpy r2, r5
108be3e0  0110a0e3  mov r1, #0x1
108be3e4  0700a0e1  cpy r0, r7
108be3e8  5639feeb  bl 0x1084c948
108be3ec  0520a0e1  cpy r2, r5
108be3f0  1310a0e3  mov r1, #0x13
108be3f4  0700a0e1  cpy r0, r7
108be3f8  dfaefeeb  bl 0x10869f7c
108be3fc  0010a0e1  cpy r1, r0
108be400  1f0004e2  and r0, r4, #0x1f
108be404  0650c1e5  strb r5, [r1,#0x6]
108be408  800280e1  orr r0, r0, r0, lsl #0x5
108be40c  120b80e3  orr r0, r0, #0x4800
108be410  b400c1e1  strh r0, [r1,#0x4]
108be414  0700a0e1  cpy r0, r7
108be418  caa5feeb  bl 0x10867b48
108be41c  0520a0e1  cpy r2, r5
108be420  0700a0e1  cpy r0, r7
108be424  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108be428  0210a0e3  mov r1, #0x2
108be42c  4539feea  b 0x1084c948
108be430  014084e2  add r4, r4, #0x1
108be434  040056e1  cmp r6, r4
108be438  c8ffff8a  bhi 0x108be360
108be43c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
