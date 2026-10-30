1086b9fc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1086ba00  0060b0e1  movs r6, r0
1086ba04  01005613  cmpne r6, #0x1
1086ba08  0170a0e1  cpy r7, r1
1086ba0c  1f00000a  beq 0x1086ba90
1086ba10  a4509fe5  ldr r5, [0x1086babc]
1086ba14  0080a0e3  mov r8, #0x0
1086ba18  004095e5  ldr r4, [r5,#0x0]
1086ba1c  600054e3  cmp r4, #0x60
1086ba20  0b00002a  bcs 0x1086ba54
1086ba24  840085e0  add r0, r5, r4, lsl #0x1
1086ba28  b400d0e1  ldrh r0, [r0,#0x4]
1086ba2c  000050e3  cmp r0, #0x0
1086ba30  0500000a  beq 0x1086ba4c
1086ba34  060050e1  cmp r0, r6
1086ba38  1b00001a  bne 0x1086baac
1086ba3c  0020a0e3  mov r2, #0x0
1086ba40  0210a0e1  cpy r1, r2
1086ba44  0200a0e1  cpy r0, r2
1086ba48  5152feeb  bl 0x10800394
1086ba4c  600054e3  cmp r4, #0x60
1086ba50  0400003a  bcc 0x1086ba68
1086ba54  0020a0e3  mov r2, #0x0
1086ba58  0210a0e1  cpy r1, r2
1086ba5c  0200a0e1  cpy r0, r2
1086ba60  4b52feeb  bl 0x10800394
1086ba64  0840a0e1  cpy r4, r8
1086ba68  042085e0  add r2, r5, r4
1086ba6c  843085e0  add r3, r5, r4, lsl #0x1
1086ba70  040185e0  add r0, r5, r4, lsl #0x2
1086ba74  b460c3e1  strh r6, [r3,#0x4]
1086ba78  c47080e5  str r7, [r0,#0xc4]
1086ba7c  c48380e5  str r8, [r0,#0x3c4]
1086ba80  0110a0e3  mov r1, #0x1
1086ba84  448280e5  str r8, [r0,#0x244]
1086ba88  4415c2e5  strb r1, [r2,#0x544]
1086ba8c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
1086ba90  0020a0e3  mov r2, #0x0
1086ba94  0210a0e1  cpy r1, r2
1086ba98  0200a0e1  cpy r0, r2
1086ba9c  3c52feeb  bl 0x10800394
1086baa0  00f020e3  nop
1086baa4  00f020e3  nop
1086baa8  d8ffffea  b 0x1086ba10
1086baac  014084e2  add r4, r4, #0x1
1086bab0  600054e3  cmp r4, #0x60
1086bab4  daffff3a  bcc 0x1086ba24
1086bab8  e5ffffea  b 0x1086ba54
