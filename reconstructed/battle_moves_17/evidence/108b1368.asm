108b1368  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b136c  0260a0e1  cpy r6, r2
108b1370  0170a0e1  cpy r7, r1
108b1374  0300a0e3  mov r0, #0x3
108b1378  67e9feeb  bl 0x1086b91c
108b137c  060050e1  cmp r0, r6
108b1380  1c00001a  bne 0x108b13f8
108b1384  0610a0e1  cpy r1, r6
108b1388  0700a0e1  cpy r0, r7
108b138c  438cfeeb  bl 0x108544a0
108b1390  ef5c00eb  bl 0x108c8754
108b1394  010050e3  cmp r0, #0x1
108b1398  0050a0e1  cpy r5, r0
108b139c  1500009a  bls 0x108b13f8
108b13a0  0620a0e1  cpy r2, r6
108b13a4  2e10a0e3  mov r1, #0x2e
108b13a8  0700a0e1  cpy r0, r7
108b13ac  f2e2feeb  bl 0x10869f7c
108b13b0  0040a0e1  cpy r4, r0
108b13b4  40009fe5  ldr r0, [0x108b13fc]
108b13b8  0460c4e5  strb r6, [r4,#0x4]
108b13bc  3c209fe5  ldr r2, [0x108b1400]
108b13c0  000055e1  cmp r5, r0
108b13c4  0100a093  movls r0, #0x1
108b13c8  fa0f4582  subhi r0, r5, #0x3e8
108b13cc  b600c4e1  strh r0, [r4,#0x6]
108b13d0  0210a0e3  mov r1, #0x2
108b13d4  080084e2  add r0, r4, #0x8
108b13d8  bd6ffeeb  bl 0x1084d2d4
108b13dc  0610a0e1  cpy r1, r6
108b13e0  080084e2  add r0, r4, #0x8
108b13e4  ac6ffeeb  bl 0x1084d29c
108b13e8  0410a0e1  cpy r1, r4
108b13ec  0700a0e1  cpy r0, r7
108b13f0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b13f4  d3d9feea  b 0x10867b48
108b13f8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
