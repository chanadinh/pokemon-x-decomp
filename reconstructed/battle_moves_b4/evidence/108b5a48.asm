108b5a48  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b5a4c  0300a0e3  mov r0, #0x3
108b5a50  0280a0e1  cpy r8, r2
108b5a54  0190a0e1  cpy r9, r1
108b5a58  afd7feeb  bl 0x1086b91c
108b5a5c  080050e1  cmp r0, r8
108b5a60  4600001a  bne 0x108b5b80
108b5a64  0600a0e3  mov r0, #0x6
108b5a68  abd7feeb  bl 0x1086b91c
108b5a6c  ff7000e2  and r7, r0, #0xff
108b5a70  0710a0e1  cpy r1, r7
108b5a74  0900a0e1  cpy r0, r9
108b5a78  887afeeb  bl 0x108544a0
108b5a7c  00a0a0e1  cpy r10, r0
108b5a80  0060a0e3  mov r6, #0x0
108b5a84  1c10a0e3  mov r1, #0x1c
108b5a88  0900a0e1  cpy r0, r9
108b5a8c  4f5bfeeb  bl 0x1084c7d0
108b5a90  0050a0e1  cpy r5, r0
108b5a94  0040a0e3  mov r4, #0x0
108b5a98  010084e2  add r0, r4, #0x1
108b5a9c  ff1000e2  and r1, r0, #0xff
108b5aa0  0a00a0e1  cpy r0, r10
108b5aa4  5dfafeeb  bl 0x10874420
108b5aa8  060050e3  cmp r0, #0x6
108b5aac  040185e7  str r0, [r5,r4,lsl #0x2]
108b5ab0  0300000a  beq 0x108b5ac4
108b5ab4  060040e2  sub r0, r0, #0x6
108b5ab8  060060e2  rsb r0, r0, #0x6
108b5abc  0160a0e3  mov r6, #0x1
108b5ac0  040185e7  str r0, [r5,r4,lsl #0x2]
108b5ac4  014084e2  add r4, r4, #0x1
108b5ac8  070054e3  cmp r4, #0x7
108b5acc  f1ffff3a  bcc 0x108b5a98
108b5ad0  000056e3  cmp r6, #0x0
108b5ad4  2900000a  beq 0x108b5b80
108b5ad8  0820a0e1  cpy r2, r8
108b5adc  0e10a0e3  mov r1, #0xe
108b5ae0  0900a0e1  cpy r0, r9
108b5ae4  24d1feeb  bl 0x10869f7c
108b5ae8  0040a0e1  cpy r4, r0
108b5aec  0470c0e5  strb r7, [r0,#0x4]
108b5af0  000095e5  ldr r0, [r5,#0x0]
108b5af4  0500c4e5  strb r0, [r4,#0x5]
108b5af8  040095e5  ldr r0, [r5,#0x4]
108b5afc  0600c4e5  strb r0, [r4,#0x6]
108b5b00  080095e5  ldr r0, [r5,#0x8]
108b5b04  0700c4e5  strb r0, [r4,#0x7]
108b5b08  0c0095e5  ldr r0, [r5,#0xc]
108b5b0c  0800c4e5  strb r0, [r4,#0x8]
108b5b10  100095e5  ldr r0, [r5,#0x10]
108b5b14  0900c4e5  strb r0, [r4,#0x9]
108b5b18  140095e5  ldr r0, [r5,#0x14]
108b5b1c  0a00c4e5  strb r0, [r4,#0xa]
108b5b20  180095e5  ldr r0, [r5,#0x18]
108b5b24  0b00c4e5  strb r0, [r4,#0xb]
108b5b28  0a00a0e1  cpy r0, r10
108b5b2c  5501d0e5  ldrb r0, [r0,#0x155]
108b5b30  0c00c4e5  strb r0, [r4,#0xc]
108b5b34  0410a0e1  cpy r1, r4
108b5b38  0900a0e1  cpy r0, r9
108b5b3c  01c8feeb  bl 0x10867b48
108b5b40  0820a0e1  cpy r2, r8
108b5b44  0310a0e3  mov r1, #0x3
108b5b48  0900a0e1  cpy r0, r9
108b5b4c  0ad1feeb  bl 0x10869f7c
108b5b50  0040a0e1  cpy r4, r0
108b5b54  28209fe5  ldr r2, [0x108b5b84]
108b5b58  0210a0e3  mov r1, #0x2
108b5b5c  040080e2  add r0, r0, #0x4
108b5b60  db5dfeeb  bl 0x1084d2d4
108b5b64  0710a0e1  cpy r1, r7
108b5b68  040084e2  add r0, r4, #0x4
108b5b6c  ca5dfeeb  bl 0x1084d29c
108b5b70  0410a0e1  cpy r1, r4
108b5b74  0900a0e1  cpy r0, r9
108b5b78  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b5b7c  f1c7feea  b 0x10867b48
108b5b80  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
