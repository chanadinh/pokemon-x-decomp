108af56c  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108af570  0260a0e1  cpy r6, r2
108af574  0170a0e1  cpy r7, r1
108af578  0340a0e1  cpy r4, r3
108af57c  0300a0e3  mov r0, #0x3
108af580  e5f0feeb  bl 0x1086b91c
108af584  060050e1  cmp r0, r6
108af588  3e00001a  bne 0x108af688
108af58c  0700a0e1  cpy r0, r7
108af590  0610a0e1  cpy r1, r6
108af594  c193feeb  bl 0x108544a0
108af598  b201d0e1  ldrh r0, [r0,#0x12]
108af59c  000050e3  cmp r0, #0x0
108af5a0  00009405  ldreq r0, [r4,#0x0]
108af5a4  00005003  cmpeq r0, #0x0
108af5a8  0190a003  moveq r9, #0x1
108af5ac  3500001a  bne 0x108af688
108af5b0  0600a0e3  mov r0, #0x6
108af5b4  d8f0feeb  bl 0x1086b91c
108af5b8  ff5000e2  and r5, r0, #0xff
108af5bc  1f0055e3  cmp r5, #0x1f
108af5c0  3000000a  beq 0x108af688
108af5c4  0510a0e1  cpy r1, r5
108af5c8  0700a0e1  cpy r0, r7
108af5cc  b393feeb  bl 0x108544a0
108af5d0  0080a0e1  cpy r8, r0
108af5d4  b201d0e1  ldrh r0, [r0,#0x12]
108af5d8  000050e3  cmp r0, #0x0
108af5dc  2900000a  beq 0x108af688
108af5e0  0520a0e1  cpy r2, r5
108af5e4  0610a0e1  cpy r1, r6
108af5e8  0700a0e1  cpy r0, r7
108af5ec  5a71feeb  bl 0x1084bb5c
108af5f0  000050e3  cmp r0, #0x0
108af5f4  2300001a  bne 0x108af688
108af5f8  0620a0e1  cpy r2, r6
108af5fc  2510a0e3  mov r1, #0x25
108af600  0700a0e1  cpy r0, r7
108af604  5ceafeeb  bl 0x10869f7c
108af608  0040a0e1  cpy r4, r0
108af60c  0450c0e5  strb r5, [r0,#0x4]
108af610  74209fe5  ldr r2, [0x108af68c]
108af614  0210a0e3  mov r1, #0x2
108af618  080080e2  add r0, r0, #0x8
108af61c  2c77feeb  bl 0x1084d2d4
108af620  0610a0e1  cpy r1, r6
108af624  080084e2  add r0, r4, #0x8
108af628  1b77feeb  bl 0x1084d29c
108af62c  0510a0e1  cpy r1, r5
108af630  080084e2  add r0, r4, #0x8
108af634  1877feeb  bl 0x1084d29c
108af638  0800a0e1  cpy r0, r8
108af63c  b201d0e1  ldrh r0, [r0,#0x12]
108af640  0010a0e1  cpy r1, r0
108af644  080084e2  add r0, r4, #0x8
108af648  1377feeb  bl 0x1084d29c
108af64c  0410a0e1  cpy r1, r4
108af650  0700a0e1  cpy r0, r7
108af654  3be1feeb  bl 0x10867b48
108af658  0620a0e1  cpy r2, r6
108af65c  3b10a0e3  mov r1, #0x3b
108af660  0700a0e1  cpy r0, r7
108af664  44eafeeb  bl 0x10869f7c
108af668  0010a0e1  cpy r1, r0
108af66c  000090e5  ldr r0, [r0,#0x0]
108af670  0490c1e5  strb r9, [r1,#0x4]
108af674  010480e3  orr r0, r0, #0x1000000
108af678  000081e5  str r0, [r1,#0x0]
108af67c  0700a0e1  cpy r0, r7
108af680  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108af684  2fe1feea  b 0x10867b48
108af688  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
