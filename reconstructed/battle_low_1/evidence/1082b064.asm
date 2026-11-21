1082b064  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1082b068  0140a0e1  cpy r4, r1
1082b06c  0070a0e3  mov r7, #0x0
1082b070  18009de5  ldr r0, [sp,#0x18]
1082b074  000050e3  cmp r0, #0x0
1082b078  1700000a  beq 0x1082b0dc
1082b07c  0510d4e5  ldrb r1, [r4,#0x5]
1082b080  0200b0e1  movs r0, r2
1082b084  b000c4e1  strh r0, [r4,#0x0]
1082b088  ff10c1e3  bic r1, r1, #0xff
1082b08c  0510c4e5  strb r1, [r4,#0x5]
1082b090  0350a0e1  cpy r5, r3
1082b094  0370c405  strbeq r7, [r4,#0x3]
1082b098  0200000a  beq 0x1082b0a8
1082b09c  0010a0e3  mov r1, #0x0
1082b0a0  3756ffeb  bl 0x10800984
1082b0a4  0300c4e5  strb r0, [r4,#0x3]
1082b0a8  000055e3  cmp r5, #0x0
1082b0ac  0300d415  ldrbne r0, [r4,#0x3]
1082b0b0  05005011  cmpne r0, r5
1082b0b4  0350c485  strbhi r5, [r4,#0x3]
1082b0b8  0310d4e5  ldrb r1, [r4,#0x3]
1082b0bc  0210c4e5  strb r1, [r4,#0x2]
1082b0c0  0c00d4e5  ldrb r0, [r4,#0xc]
1082b0c4  000050e3  cmp r0, #0x0
1082b0c8  1600000a  beq 0x1082b128
1082b0cc  060084e2  add r0, r4, #0x6
1082b0d0  0410a0e1  cpy r1, r4
1082b0d4  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1082b0d8  6bd200ea  b 0x1085fa8c
1082b0dc  0b10d4e5  ldrb r1, [r4,#0xb]
1082b0e0  0200b0e1  movs r0, r2
1082b0e4  065084e2  add r5, r4, #0x6
1082b0e8  ff10c1e3  bic r1, r1, #0xff
1082b0ec  b600c4e1  strh r0, [r4,#0x6]
1082b0f0  0b10c4e5  strb r1, [r4,#0xb]
1082b0f4  0360a0e1  cpy r6, r3
1082b0f8  0370c505  strbeq r7, [r5,#0x3]
1082b0fc  0200000a  beq 0x1082b10c
1082b100  0010a0e3  mov r1, #0x0
1082b104  1e56ffeb  bl 0x10800984
1082b108  0300c5e5  strb r0, [r5,#0x3]
1082b10c  000056e3  cmp r6, #0x0
1082b110  0300d515  ldrbne r0, [r5,#0x3]
1082b114  06005011  cmpne r0, r6
1082b118  0360c585  strbhi r6, [r5,#0x3]
1082b11c  0300d5e5  ldrb r0, [r5,#0x3]
1082b120  0200c5e5  strb r0, [r5,#0x2]
1082b124  0c70c4e5  strb r7, [r4,#0xc]
1082b128  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
