108babcc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108babd0  0250a0e1  cpy r5, r2
108babd4  0080a0e1  cpy r8, r0
108babd8  0170a0e1  cpy r7, r1
108babdc  0400a0e3  mov r0, #0x4
108babe0  4dc3feeb  bl 0x1086b91c
108babe4  050050e1  cmp r0, r5
108babe8  4100001a  bne 0x108bacf4
108babec  0510a0e1  cpy r1, r5
108babf0  0700a0e1  cpy r0, r7
108babf4  2966feeb  bl 0x108544a0
108babf8  1193feeb  bl 0x1085f844
108babfc  000050e3  cmp r0, #0x0
108bac00  3b00000a  beq 0x108bacf4
108bac04  1200a0e3  mov r0, #0x12
108bac08  43c3feeb  bl 0x1086b91c
108bac0c  7000ffe6  uxth r0, r0
108bac10  fc46feeb  bl 0x1084c808
108bac14  000050e3  cmp r0, #0x0
108bac18  3200001a  bne 0x108bace8
108bac1c  0300a0e3  mov r0, #0x3
108bac20  3dc3feeb  bl 0x1086b91c
108bac24  ff6000e2  and r6, r0, #0xff
108bac28  0610a0e1  cpy r1, r6
108bac2c  0700a0e1  cpy r0, r7
108bac30  1a66feeb  bl 0x108544a0
108bac34  0040a0e1  cpy r4, r0
108bac38  0610a0e1  cpy r1, r6
108bac3c  0500a0e1  cpy r0, r5
108bac40  5846feeb  bl 0x1084c5a8
108bac44  000050e3  cmp r0, #0x0
108bac48  2600001a  bne 0x108bace8
108bac4c  0400a0e1  cpy r0, r4
108bac50  fb92feeb  bl 0x1085f844
108bac54  000050e3  cmp r0, #0x0
108bac58  2200001a  bne 0x108bace8
108bac5c  1400a0e3  mov r0, #0x14
108bac60  2dc3feeb  bl 0x1086b91c
108bac64  0040a0e1  cpy r4, r0
108bac68  0800a0e1  cpy r0, r8
108bac6c  a346feeb  bl 0x1084c700
108bac70  0620a0e1  cpy r2, r6
108bac74  1210a0e3  mov r1, #0x12
108bac78  0700a0e1  cpy r0, r7
108bac7c  bebcfeeb  bl 0x10869f7c
108bac80  0010a0e1  cpy r1, r0
108bac84  0100a0e3  mov r0, #0x1
108bac88  b640c1e1  strh r4, [r1,#0x6]
108bac8c  0450c1e5  strb r5, [r1,#0x4]
108bac90  0500c1e5  strb r0, [r1,#0x5]
108bac94  0700a0e1  cpy r0, r7
108bac98  aab3feeb  bl 0x10867b48
108bac9c  0520a0e1  cpy r2, r5
108baca0  1210a0e3  mov r1, #0x12
108baca4  0700a0e1  cpy r0, r7
108baca8  b3bcfeeb  bl 0x10869f7c
108bacac  0040a0e1  cpy r4, r0
108bacb0  0460c0e5  strb r6, [r0,#0x4]
108bacb4  0800a0e1  cpy r0, r8
108bacb8  b803d0e1  ldrh r0, [r0,#0x38]
108bacbc  b600c4e1  strh r0, [r4,#0x6]
108bacc0  30209fe5  ldr r2, [0x108bacf8]
108bacc4  0210a0e3  mov r1, #0x2
108bacc8  080084e2  add r0, r4, #0x8
108baccc  8049feeb  bl 0x1084d2d4
108bacd0  0510a0e1  cpy r1, r5
108bacd4  080084e2  add r0, r4, #0x8
108bacd8  6f49feeb  bl 0x1084d29c
108bacdc  0410a0e1  cpy r1, r4
108bace0  0700a0e1  cpy r0, r7
108bace4  97b3feeb  bl 0x10867b48
108bace8  0800a0e1  cpy r0, r8
108bacec  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bacf0  3549feea  b 0x1084d1cc
108bacf4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
