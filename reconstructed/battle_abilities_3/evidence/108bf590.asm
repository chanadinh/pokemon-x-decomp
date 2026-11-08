108bf590  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bf594  0200a0e3  mov r0, #0x2
108bf598  0180a0e1  cpy r8, r1
108bf59c  0270a0e1  cpy r7, r2
108bf5a0  ddb0feeb  bl 0x1086b91c
108bf5a4  070050e1  cmp r0, r7
108bf5a8  6600001a  bne 0x108bf748
108bf5ac  0710a0e1  cpy r1, r7
108bf5b0  0800a0e1  cpy r0, r8
108bf5b4  b953feeb  bl 0x108544a0
108bf5b8  00a0a0e1  cpy r10, r0
108bf5bc  b201d0e1  ldrh r0, [r0,#0x12]
108bf5c0  000050e3  cmp r0, #0x0
108bf5c4  5f00001a  bne 0x108bf748
108bf5c8  0710a0e1  cpy r1, r7
108bf5cc  0800a0e1  cpy r0, r8
108bf5d0  6835feeb  bl 0x1084cb78
108bf5d4  024c80e3  orr r4, r0, #0x200
108bf5d8  0e10a0e3  mov r1, #0xe
108bf5dc  0800a0e1  cpy r0, r8
108bf5e0  7a34feeb  bl 0x1084c7d0
108bf5e4  0050a0e1  cpy r5, r0
108bf5e8  0020a0e1  cpy r2, r0
108bf5ec  0410a0e1  cpy r1, r4
108bf5f0  0800a0e1  cpy r0, r8
108bf5f4  8931feeb  bl 0x1084bc20
108bf5f8  0010a0e3  mov r1, #0x0
108bf5fc  0c00c5e5  strb r0, [r5,#0xc]
108bf600  000050e3  cmp r0, #0x0
108bf604  0160a0e1  cpy r6, r1
108bf608  0140a0e1  cpy r4, r1
108bf60c  0d10c5e5  strb r1, [r5,#0xd]
108bf610  4c00009a  bls 0x108bf748
108bf614  0410d5e7  ldrb r1, [r5,r4]
108bf618  0800a0e1  cpy r0, r8
108bf61c  9f53feeb  bl 0x108544a0
108bf620  0090a0e1  cpy r9, r0
108bf624  0810a0e3  mov r1, #0x8
108bf628  7a34feeb  bl 0x1084c818
108bf62c  000050e3  cmp r0, #0x0
108bf630  00f020e3  nop
108bf634  0a00000a  beq 0x108bf664
108bf638  0900a0e1  cpy r0, r9
108bf63c  b401d0e1  ldrh r0, [r0,#0x14]
108bf640  000050e3  cmp r0, #0x0
108bf644  00f020e3  nop
108bf648  0500000a  beq 0x108bf664
108bf64c  0900a0e1  cpy r0, r9
108bf650  1900d0e5  ldrb r0, [r0,#0x19]
108bf654  062085e0  add r2, r5, r6
108bf658  011086e2  add r1, r6, #0x1
108bf65c  ff6001e2  and r6, r1, #0xff
108bf660  0600c2e5  strb r0, [r2,#0x6]
108bf664  0c00d5e5  ldrb r0, [r5,#0xc]
108bf668  011084e2  add r1, r4, #0x1
108bf66c  ff4001e2  and r4, r1, #0xff
108bf670  040050e1  cmp r0, r4
108bf674  e6ffff8a  bhi 0x108bf614
108bf678  000056e3  cmp r6, #0x0
108bf67c  3100000a  beq 0x108bf748
108bf680  0600a0e1  cpy r0, r6
108bf684  4c34feeb  bl 0x1084c7bc
108bf688  ff0000e2  and r0, r0, #0xff
108bf68c  050080e0  add r0, r0, r5
108bf690  0660d0e5  ldrb r6, [r0,#0x6]
108bf694  0800a0e1  cpy r0, r8
108bf698  0610a0e1  cpy r1, r6
108bf69c  7f53feeb  bl 0x108544a0
108bf6a0  00f020e3  nop
108bf6a4  00f020e3  nop
108bf6a8  b401d0e1  ldrh r0, [r0,#0x14]
108bf6ac  0050b0e1  movs r5, r0
108bf6b0  00f020e3  nop
108bf6b4  2300000a  beq 0x108bf748
108bf6b8  0a00a0e1  cpy r0, r10
108bf6bc  bc00d0e1  ldrh r0, [r0,#0xc]
108bf6c0  0010a0e1  cpy r1, r0
108bf6c4  0520a0e1  cpy r2, r5
108bf6c8  0800a0e1  cpy r0, r8
108bf6cc  5253feeb  bl 0x1085441c
108bf6d0  000050e3  cmp r0, #0x0
108bf6d4  00f020e3  nop
108bf6d8  1a00001a  bne 0x108bf748
108bf6dc  0720a0e1  cpy r2, r7
108bf6e0  2110a0e3  mov r1, #0x21
108bf6e4  0800a0e1  cpy r0, r8
108bf6e8  23aafeeb  bl 0x10869f7c
108bf6ec  0040a0e1  cpy r4, r0
108bf6f0  000090e5  ldr r0, [r0,#0x0]
108bf6f4  0960c4e5  strb r6, [r4,#0x9]
108bf6f8  0110a0e3  mov r1, #0x1
108bf6fc  0670c4e5  strb r7, [r4,#0x6]
108bf700  0810c4e5  strb r1, [r4,#0x8]
108bf704  020580e3  orr r0, r0, #0x800000
108bf708  b450c4e1  strh r5, [r4,#0x4]
108bf70c  000084e5  str r0, [r4,#0x0]
108bf710  34209fe5  ldr r2, [0x108bf74c]
108bf714  0210a0e3  mov r1, #0x2
108bf718  0c0084e2  add r0, r4, #0xc
108bf71c  ec36feeb  bl 0x1084d2d4
108bf720  0710a0e1  cpy r1, r7
108bf724  0c0084e2  add r0, r4, #0xc
108bf728  db36feeb  bl 0x1084d29c
108bf72c  0510a0e1  cpy r1, r5
108bf730  0c0084e2  add r0, r4, #0xc
108bf734  d836feeb  bl 0x1084d29c
108bf738  0410a0e1  cpy r1, r4
108bf73c  0800a0e1  cpy r0, r8
108bf740  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bf744  ffa0feea  b 0x10867b48
108bf748  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
