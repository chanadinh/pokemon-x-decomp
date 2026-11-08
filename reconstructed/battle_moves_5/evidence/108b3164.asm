108b3164  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b3168  0160a0e1  cpy r6, r1
108b316c  0250a0e1  cpy r5, r2
108b3170  0300a0e3  mov r0, #0x3
108b3174  e8e1feeb  bl 0x1086b91c
108b3178  050050e1  cmp r0, r5
108b317c  2100001a  bne 0x108b3208
108b3180  0110a0e3  mov r1, #0x1
108b3184  0600a0e1  cpy r0, r6
108b3188  7964feeb  bl 0x1084c374
108b318c  000050e3  cmp r0, #0x0
108b3190  0170a0e3  mov r7, #0x1
108b3194  0520a0e1  cpy r2, r5
108b3198  0700000a  beq 0x108b31bc
108b319c  1d10a0e3  mov r1, #0x1d
108b31a0  0600a0e1  cpy r0, r6
108b31a4  74dbfeeb  bl 0x10869f7c
108b31a8  0010a0e1  cpy r1, r0
108b31ac  0470c0e5  strb r7, [r0,#0x4]
108b31b0  0600a0e1  cpy r0, r6
108b31b4  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b31b8  62d2feea  b 0x10867b48
108b31bc  1c10a0e3  mov r1, #0x1c
108b31c0  0600a0e1  cpy r0, r6
108b31c4  6cdbfeeb  bl 0x10869f7c
108b31c8  0040a0e1  cpy r4, r0
108b31cc  0470c0e5  strb r7, [r0,#0x4]
108b31d0  0500a0e3  mov r0, #0x5
108b31d4  1863feeb  bl 0x1084be3c
108b31d8  080084e5  str r0, [r4,#0x8]
108b31dc  d72fa0e3  mov r2, #0x35c
108b31e0  0210a0e3  mov r1, #0x2
108b31e4  0c0084e2  add r0, r4, #0xc
108b31e8  3968feeb  bl 0x1084d2d4
108b31ec  0510a0e1  cpy r1, r5
108b31f0  0c0084e2  add r0, r4, #0xc
108b31f4  2868feeb  bl 0x1084d29c
108b31f8  0410a0e1  cpy r1, r4
108b31fc  0600a0e1  cpy r0, r6
108b3200  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b3204  4fd2feea  b 0x10867b48
108b3208  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
