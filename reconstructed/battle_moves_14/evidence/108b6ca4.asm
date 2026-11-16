108b6ca4  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b6ca8  0160a0e1  cpy r6, r1
108b6cac  0070a0e1  cpy r7, r0
108b6cb0  0240a0e1  cpy r4, r2
108b6cb4  0300a0e3  mov r0, #0x3
108b6cb8  17d3feeb  bl 0x1086b91c
108b6cbc  040050e1  cmp r0, r4
108b6cc0  3000001a  bne 0x108b6d88
108b6cc4  0d00a0e3  mov r0, #0xd
108b6cc8  13d3feeb  bl 0x1086b91c
108b6ccc  ff5000e2  and r5, r0, #0xff
108b6cd0  0680a0e1  cpy r8, r6
108b6cd4  0300a0e3  mov r0, #0x3
108b6cd8  0fd3feeb  bl 0x1086b91c
108b6cdc  040050e1  cmp r0, r4
108b6ce0  2800001a  bne 0x108b6d88
108b6ce4  0110a0e3  mov r1, #0x1
108b6ce8  5300a0e3  mov r0, #0x53
108b6cec  f8bafeeb  bl 0x108658d4
108b6cf0  000050e3  cmp r0, #0x0
108b6cf4  2300000a  beq 0x108b6d88
108b6cf8  0410a0e1  cpy r1, r4
108b6cfc  0800a0e1  cpy r0, r8
108b6d00  14f1feeb  bl 0x10873158
108b6d04  0a0055e3  cmp r5, #0xa
108b6d08  0060a0e1  cpy r6, r0
108b6d0c  0800001a  bne 0x108b6d34
108b6d10  0800a0e1  cpy r0, r8
108b6d14  7e54feeb  bl 0x1084bf14
108b6d18  0020a0e3  mov r2, #0x0
108b6d1c  0610a0e1  cpy r1, r6
108b6d20  3ddcfeeb  bl 0x1086de1c
108b6d24  0050a0e1  cpy r5, r0
108b6d28  0010a0e1  cpy r1, r0
108b6d2c  0d00a0e3  mov r0, #0xd
108b6d30  e7bafeeb  bl 0x108658d4
108b6d34  0420a0e1  cpy r2, r4
108b6d38  1f10a0e3  mov r1, #0x1f
108b6d3c  0800a0e1  cpy r0, r8
108b6d40  8dccfeeb  bl 0x10869f7c
108b6d44  0390a0e3  mov r9, #0x3
108b6d48  0550c0e5  strb r5, [r0,#0x5]
108b6d4c  0040a0e1  cpy r4, r0
108b6d50  0490c0e5  strb r9, [r0,#0x4]
108b6d54  0800a0e1  cpy r0, r8
108b6d58  8159feeb  bl 0x1084d364
108b6d5c  020080e2  add r0, r0, #0x2
108b6d60  080084e5  str r0, [r4,#0x8]
108b6d64  0700a0e1  cpy r0, r7
108b6d68  b803d0e1  ldrh r0, [r0,#0x38]
108b6d6c  0c0084e5  str r0, [r4,#0xc]
108b6d70  1890c4e5  strb r9, [r4,#0x18]
108b6d74  0410a0e1  cpy r1, r4
108b6d78  0800a0e1  cpy r0, r8
108b6d7c  106084e5  str r6, [r4,#0x10]
108b6d80  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b6d84  6fc3feea  b 0x10867b48
108b6d88  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
