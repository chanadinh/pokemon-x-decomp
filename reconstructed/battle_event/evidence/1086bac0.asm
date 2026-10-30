1086bac0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1086bac4  0060b0e1  movs r6, r0
1086bac8  01005613  cmpne r6, #0x1
1086bacc  0170a0e1  cpy r7, r1
1086bad0  1f00000a  beq 0x1086bb54
1086bad4  a4509fe5  ldr r5, [0x1086bb80]
1086bad8  0080a0e3  mov r8, #0x0
1086badc  004095e5  ldr r4, [r5,#0x0]
1086bae0  600054e3  cmp r4, #0x60
1086bae4  0b00002a  bcs 0x1086bb18
1086bae8  840085e0  add r0, r5, r4, lsl #0x1
1086baec  b400d0e1  ldrh r0, [r0,#0x4]
1086baf0  000050e3  cmp r0, #0x0
1086baf4  0500000a  beq 0x1086bb10
1086baf8  060050e1  cmp r0, r6
1086bafc  1b00001a  bne 0x1086bb70
1086bb00  0020a0e3  mov r2, #0x0
1086bb04  0210a0e1  cpy r1, r2
1086bb08  0200a0e1  cpy r0, r2
1086bb0c  2052feeb  bl 0x10800394
1086bb10  600054e3  cmp r4, #0x60
1086bb14  0400003a  bcc 0x1086bb2c
1086bb18  0020a0e3  mov r2, #0x0
1086bb1c  0210a0e1  cpy r1, r2
1086bb20  0200a0e1  cpy r0, r2
1086bb24  1a52feeb  bl 0x10800394
1086bb28  0840a0e1  cpy r4, r8
1086bb2c  042085e0  add r2, r5, r4
1086bb30  843085e0  add r3, r5, r4, lsl #0x1
1086bb34  040185e0  add r0, r5, r4, lsl #0x2
1086bb38  b460c3e1  strh r6, [r3,#0x4]
1086bb3c  c47080e5  str r7, [r0,#0xc4]
1086bb40  c48380e5  str r8, [r0,#0x3c4]
1086bb44  0410a0e3  mov r1, #0x4
1086bb48  448280e5  str r8, [r0,#0x244]
1086bb4c  4415c2e5  strb r1, [r2,#0x544]
1086bb50  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
1086bb54  0020a0e3  mov r2, #0x0
1086bb58  0210a0e1  cpy r1, r2
1086bb5c  0200a0e1  cpy r0, r2
1086bb60  0b52feeb  bl 0x10800394
1086bb64  00f020e3  nop
1086bb68  00f020e3  nop
1086bb6c  d8ffffea  b 0x1086bad4
1086bb70  014084e2  add r4, r4, #0x1
1086bb74  600054e3  cmp r4, #0x60
1086bb78  daffff3a  bcc 0x1086bae8
1086bb7c  e5ffffea  b 0x1086bb18
