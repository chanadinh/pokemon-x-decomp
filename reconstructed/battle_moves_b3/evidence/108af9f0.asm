108af9f0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108af9f4  0250a0e1  cpy r5, r2
108af9f8  0170a0e1  cpy r7, r1
108af9fc  0300a0e3  mov r0, #0x3
108afa00  c5effeeb  bl 0x1086b91c
108afa04  050050e1  cmp r0, r5
108afa08  2100001a  bne 0x108afa94
108afa0c  0510a0e1  cpy r1, r5
108afa10  0700a0e1  cpy r0, r7
108afa14  a192feeb  bl 0x108544a0
108afa18  0040a0e1  cpy r4, r0
108afa1c  b201d0e1  ldrh r0, [r0,#0x12]
108afa20  000050e3  cmp r0, #0x0
108afa24  1a00001a  bne 0x108afa94
108afa28  0400a0e1  cpy r0, r4
108afa2c  b401d0e1  ldrh r0, [r0,#0x14]
108afa30  0060b0e1  movs r6, r0
108afa34  1600000a  beq 0x108afa94
108afa38  0520a0e1  cpy r2, r5
108afa3c  2110a0e3  mov r1, #0x21
108afa40  0700a0e1  cpy r0, r7
108afa44  4ce9feeb  bl 0x10869f7c
108afa48  0040a0e1  cpy r4, r0
108afa4c  0100a0e3  mov r0, #0x1
108afa50  0650c4e5  strb r5, [r4,#0x6]
108afa54  b460c4e1  strh r6, [r4,#0x4]
108afa58  b72f80e2  add r2, r0, #0x2dc
108afa5c  0700c4e5  strb r0, [r4,#0x7]
108afa60  0210a0e3  mov r1, #0x2
108afa64  0c0084e2  add r0, r4, #0xc
108afa68  1976feeb  bl 0x1084d2d4
108afa6c  0510a0e1  cpy r1, r5
108afa70  0c0084e2  add r0, r4, #0xc
108afa74  0876feeb  bl 0x1084d29c
108afa78  0610a0e1  cpy r1, r6
108afa7c  0c0084e2  add r0, r4, #0xc
108afa80  0576feeb  bl 0x1084d29c
108afa84  0410a0e1  cpy r1, r4
108afa88  0700a0e1  cpy r0, r7
108afa8c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108afa90  2ce0feea  b 0x10867b48
108afa94  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
