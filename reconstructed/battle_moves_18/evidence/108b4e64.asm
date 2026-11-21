108b4e64  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b4e68  0250a0e1  cpy r5, r2
108b4e6c  0170a0e1  cpy r7, r1
108b4e70  0210a0e1  cpy r1, r2
108b4e74  0700a0e1  cpy r0, r7
108b4e78  887dfeeb  bl 0x108544a0
108b4e7c  0040a0e1  cpy r4, r0
108b4e80  bc00d0e1  ldrh r0, [r0,#0xc]
108b4e84  a20f50e3  cmp r0, #0x288
108b4e88  1900001a  bne 0x108b4ef4
108b4e8c  0300a0e3  mov r0, #0x3
108b4e90  a1dafeeb  bl 0x1086b91c
108b4e94  050050e1  cmp r0, r5
108b4e98  1500001a  bne 0x108b4ef4
108b4e9c  5301d4e5  ldrb r0, [r4,#0x153]
108b4ea0  0520a0e1  cpy r2, r5
108b4ea4  3a10a0e3  mov r1, #0x3a
108b4ea8  000050e3  cmp r0, #0x0
108b4eac  0060a013  movne r6, #0x0
108b4eb0  0160a003  moveq r6, #0x1
108b4eb4  0700a0e1  cpy r0, r7
108b4eb8  2fd4feeb  bl 0x10869f7c
108b4ebc  0560c0e5  strb r6, [r0,#0x5]
108b4ec0  0040a0e1  cpy r4, r0
108b4ec4  0450c0e5  strb r5, [r0,#0x4]
108b4ec8  de20a0e3  mov r2, #0xde
108b4ecc  0210a0e3  mov r1, #0x2
108b4ed0  080080e2  add r0, r0, #0x8
108b4ed4  fe60feeb  bl 0x1084d2d4
108b4ed8  0510a0e1  cpy r1, r5
108b4edc  080084e2  add r0, r4, #0x8
108b4ee0  ed60feeb  bl 0x1084d29c
108b4ee4  0410a0e1  cpy r1, r4
108b4ee8  0700a0e1  cpy r0, r7
108b4eec  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b4ef0  14cbfeea  b 0x10867b48
108b4ef4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
