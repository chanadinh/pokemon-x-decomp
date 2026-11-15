108c0e14  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c0e18  0260a0e1  cpy r6, r2
108c0e1c  0180a0e1  cpy r8, r1
108c0e20  0300a0e3  mov r0, #0x3
108c0e24  bcaafeeb  bl 0x1086b91c
108c0e28  060050e1  cmp r0, r6
108c0e2c  2900001a  bne 0x108c0ed8
108c0e30  0610a0e1  cpy r1, r6
108c0e34  0800a0e1  cpy r0, r8
108c0e38  984dfeeb  bl 0x108544a0
108c0e3c  0040a0e1  cpy r4, r0
108c0e40  bc00d0e1  ldrh r0, [r0,#0xc]
108c0e44  021c40e2  sub r1, r0, #0x200
108c0e48  a91051e2  subs r1, r1, #0xa9
108c0e4c  2100001a  bne 0x108c0ed8
108c0e50  1200a0e3  mov r0, #0x12
108c0e54  b0aafeeb  bl 0x1086b91c
108c0e58  5341d4e5  ldrb r4, [r4,#0x153]
108c0e5c  7000ffe6  uxth r0, r0
108c0e60  930f50e3  cmp r0, #0x24c
108c0e64  0450a0e1  cpy r5, r4
108c0e68  0040a003  moveq r4, #0x0
108c0e6c  0200000a  beq 0x108c0e7c
108c0e70  6ffdfceb  bl 0x10800434
108c0e74  000050e3  cmp r0, #0x0
108c0e78  0140a013  movne r4, #0x1
108c0e7c  040055e1  cmp r5, r4
108c0e80  1400000a  beq 0x108c0ed8
108c0e84  000054e3  cmp r4, #0x0
108c0e88  fd70a003  moveq r7, #0xfd
108c0e8c  fc70a013  movne r7, #0xfc
108c0e90  0620a0e1  cpy r2, r6
108c0e94  3a10a0e3  mov r1, #0x3a
108c0e98  0800a0e1  cpy r0, r8
108c0e9c  36a4feeb  bl 0x10869f7c
108c0ea0  0050a0e1  cpy r5, r0
108c0ea4  000090e5  ldr r0, [r0,#0x0]
108c0ea8  0460c5e5  strb r6, [r5,#0x4]
108c0eac  0540c5e5  strb r4, [r5,#0x5]
108c0eb0  020580e3  orr r0, r0, #0x800000
108c0eb4  000085e5  str r0, [r5,#0x0]
108c0eb8  7720ffe6  uxth r2, r7
108c0ebc  0110a0e3  mov r1, #0x1
108c0ec0  080085e2  add r0, r5, #0x8
108c0ec4  0231feeb  bl 0x1084d2d4
108c0ec8  0510a0e1  cpy r1, r5
108c0ecc  0800a0e1  cpy r0, r8
108c0ed0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c0ed4  1b9bfeea  b 0x10867b48
108c0ed8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
