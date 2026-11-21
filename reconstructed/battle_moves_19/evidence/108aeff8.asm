108aeff8  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108aeffc  0260a0e1  cpy r6, r2
108af000  0080a0e1  cpy r8, r0
108af004  0170a0e1  cpy r7, r1
108af008  0300a0e3  mov r0, #0x3
108af00c  42f2feeb  bl 0x1086b91c
108af010  060050e1  cmp r0, r6
108af014  4700001a  bne 0x108af138
108af018  0610a0e1  cpy r1, r6
108af01c  0700a0e1  cpy r0, r7
108af020  1e95feeb  bl 0x108544a0
108af024  0090a0e1  cpy r9, r0
108af028  0600a0e3  mov r0, #0x6
108af02c  3af2feeb  bl 0x1086b91c
108af030  ff1000e2  and r1, r0, #0xff
108af034  0700a0e1  cpy r0, r7
108af038  1895feeb  bl 0x108544a0
108af03c  0050a0e1  cpy r5, r0
108af040  ed75feeb  bl 0x1084c7fc
108af044  0040a0e1  cpy r4, r0
108af048  0500a0e1  cpy r0, r5
108af04c  8974feeb  bl 0x1084c278
108af050  040050e1  cmp r0, r4
108af054  0040a011  cpyne r4, r0
108af058  000054e3  cmp r4, #0x0
108af05c  a6005413  cmpne r4, #0xa6
108af060  3400000a  beq 0x108af138
108af064  a50054e3  cmp r4, #0xa5
108af068  070d5413  cmpne r4, #0x1c0
108af06c  3100000a  beq 0x108af138
108af070  0410a0e1  cpy r1, r4
108af074  0900a0e1  cpy r0, r9
108af078  326500eb  bl 0x108c8548
108af07c  000050e3  cmp r0, #0x0
108af080  2c00001a  bne 0x108af138
108af084  0900a0e1  cpy r0, r9
108af088  e68afeeb  bl 0x10851c28
108af08c  000050e3  cmp r0, #0x0
108af090  2800001a  bne 0x108af138
108af094  0800a0e1  cpy r0, r8
108af098  b803d0e1  ldrh r0, [r0,#0x38]
108af09c  0010a0e1  cpy r1, r0
108af0a0  0900a0e1  cpy r0, r9
108af0a4  5e74feeb  bl 0x1084c224
108af0a8  040050e3  cmp r0, #0x4
108af0ac  0050a0e1  cpy r5, r0
108af0b0  2000000a  beq 0x108af138
108af0b4  0620a0e1  cpy r2, r6
108af0b8  2610a0e3  mov r1, #0x26
108af0bc  0700a0e1  cpy r0, r7
108af0c0  adebfeeb  bl 0x10869f7c
108af0c4  0010a0e1  cpy r1, r0
108af0c8  0020a0e3  mov r2, #0x0
108af0cc  0620c1e5  strb r2, [r1,#0x6]
108af0d0  0100a0e3  mov r0, #0x1
108af0d4  0550c1e5  strb r5, [r1,#0x5]
108af0d8  0700c1e5  strb r0, [r1,#0x7]
108af0dc  0460c1e5  strb r6, [r1,#0x4]
108af0e0  0700a0e1  cpy r0, r7
108af0e4  b840c1e1  strh r4, [r1,#0x8]
108af0e8  96e2feeb  bl 0x10867b48
108af0ec  0620a0e1  cpy r2, r6
108af0f0  0310a0e3  mov r1, #0x3
108af0f4  0700a0e1  cpy r0, r7
108af0f8  9febfeeb  bl 0x10869f7c
108af0fc  0050a0e1  cpy r5, r0
108af100  2b2ea0e3  mov r2, #0x2b0
108af104  0210a0e3  mov r1, #0x2
108af108  040080e2  add r0, r0, #0x4
108af10c  7078feeb  bl 0x1084d2d4
108af110  0610a0e1  cpy r1, r6
108af114  040085e2  add r0, r5, #0x4
108af118  5f78feeb  bl 0x1084d29c
108af11c  0410a0e1  cpy r1, r4
108af120  040085e2  add r0, r5, #0x4
108af124  5c78feeb  bl 0x1084d29c
108af128  0510a0e1  cpy r1, r5
108af12c  0700a0e1  cpy r0, r7
108af130  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108af134  83e2feea  b 0x10867b48
108af138  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
