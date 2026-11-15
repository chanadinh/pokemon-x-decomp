108bd33c  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bd340  0400a0e3  mov r0, #0x4
108bd344  0270a0e1  cpy r7, r2
108bd348  0190a0e1  cpy r9, r1
108bd34c  72b9feeb  bl 0x1086b91c
108bd350  070050e1  cmp r0, r7
108bd354  3300001a  bne 0x108bd428
108bd358  1e00a0e3  mov r0, #0x1e
108bd35c  6eb9feeb  bl 0x1086b91c
108bd360  ff4000e2  and r4, r0, #0xff
108bd364  050054e3  cmp r4, #0x5
108bd368  01005413  cmpne r4, #0x1
108bd36c  04005413  cmpne r4, #0x4
108bd370  2c00001a  bne 0x108bd428
108bd374  0300a0e3  mov r0, #0x3
108bd378  67b9feeb  bl 0x1086b91c
108bd37c  ff6000e2  and r6, r0, #0xff
108bd380  1f0056e3  cmp r6, #0x1f
108bd384  07005611  cmpne r6, r7
108bd388  2600000a  beq 0x108bd428
108bd38c  0710a0e1  cpy r1, r7
108bd390  0900a0e1  cpy r0, r9
108bd394  415cfeeb  bl 0x108544a0
108bd398  00a0a0e1  cpy r10, r0
108bd39c  1f00a0e3  mov r0, #0x1f
108bd3a0  5db9feeb  bl 0x1086b91c
108bd3a4  0080a0e1  cpy r8, r0
108bd3a8  0720a0e1  cpy r2, r7
108bd3ac  0110a0e3  mov r1, #0x1
108bd3b0  0900a0e1  cpy r0, r9
108bd3b4  633dfeeb  bl 0x1084c948
108bd3b8  0720a0e1  cpy r2, r7
108bd3bc  0b10a0e3  mov r1, #0xb
108bd3c0  0900a0e1  cpy r0, r9
108bd3c4  ecb2feeb  bl 0x10869f7c
108bd3c8  0f60c0e5  strb r6, [r0,#0xf]
108bd3cc  050054e3  cmp r4, #0x5
108bd3d0  0050a0e1  cpy r5, r0
108bd3d4  b440c0e1  strh r4, [r0,#0x4]
108bd3d8  0400001a  bne 0x108bd3f0
108bd3dc  0800a0e1  cpy r0, r8
108bd3e0  76f3feeb  bl 0x1087a1c0
108bd3e4  000050e3  cmp r0, #0x0
108bd3e8  08808515  strne r8, [r5,#0x8]
108bd3ec  0300001a  bne 0x108bd400
108bd3f0  082085e2  add r2, r5, #0x8
108bd3f4  0a10a0e1  cpy r1, r10
108bd3f8  0400a0e1  cpy r0, r4
108bd3fc  0d3cfeeb  bl 0x1084c438
108bd400  0100a0e3  mov r0, #0x1
108bd404  0c00c5e5  strb r0, [r5,#0xc]
108bd408  0510a0e1  cpy r1, r5
108bd40c  0900a0e1  cpy r0, r9
108bd410  cca9feeb  bl 0x10867b48
108bd414  0720a0e1  cpy r2, r7
108bd418  0900a0e1  cpy r0, r9
108bd41c  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bd420  0210a0e3  mov r1, #0x2
108bd424  473dfeea  b 0x1084c948
108bd428  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
