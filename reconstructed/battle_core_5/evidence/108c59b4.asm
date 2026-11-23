108c59b4  f0432de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,lr}
108c59b8  0cd04de2  sub sp, sp, #0xc
108c59bc  0070a0e1  cpy r7, r0
108c59c0  0150a0e1  cpy r5, r1
108c59c4  3b40d1e5  ldrb r4, [r1,#0x3b]
108c59c8  0290a0e1  cpy r9, r2
108c59cc  0000a0e3  mov r0, #0x0
108c59d0  1f0054e3  cmp r4, #0x1f
108c59d4  0f00000a  beq 0x108c5a18
108c59d8  086097e5  ldr r6, [r7,#0x8]
108c59dc  180054e3  cmp r4, #0x18
108c59e0  0300003a  bcc 0x108c59f4
108c59e4  0020a0e3  mov r2, #0x0
108c59e8  0210a0e1  cpy r1, r2
108c59ec  0200a0e1  cpy r0, r2
108c59f0  67eafceb  bl 0x10800394
108c59f4  044186e0  add r4, r6, r4, lsl #0x2
108c59f8  840094e5  ldr r0, [r4,#0x84]
108c59fc  000050e3  cmp r0, #0x0
108c5a00  0300001a  bne 0x108c5a14
108c5a04  0020a0e3  mov r2, #0x0
108c5a08  0210a0e1  cpy r1, r2
108c5a0c  0200a0e1  cpy r0, r2
108c5a10  5feafceb  bl 0x10800394
108c5a14  840094e5  ldr r0, [r4,#0x84]
108c5a18  1010d5e5  ldrb r1, [r5,#0x10]
108c5a1c  040051e3  cmp r1, #0x4
108c5a20  0200000a  beq 0x108c5a30
108c5a24  050051e3  cmp r1, #0x5
108c5a28  0600000a  beq 0x108c5a48
108c5a2c  110000ea  b 0x108c5a78
108c5a30  000050e3  cmp r0, #0x0
108c5a34  0f00000a  beq 0x108c5a78
108c5a38  5c0090e5  ldr r0, [r0,#0x5c]
108c5a3c  070010e3  tst r0, #0x7
108c5a40  0c00000a  beq 0x108c5a78
108c5a44  390000ea  b 0x108c5b30
108c5a48  1c1097e5  ldr r1, [r7,#0x1c]
108c5a4c  901291e5  ldr r1, [r1,#0x290]
108c5a50  000051e3  cmp r1, #0x0
108c5a54  3500001a  bne 0x108c5b30
108c5a58  000050e3  cmp r0, #0x0
108c5a5c  0500000a  beq 0x108c5a78
108c5a60  681090e5  ldr r1, [r0,#0x68]
108c5a64  070011e3  tst r1, #0x7
108c5a68  3000001a  bne 0x108c5b30
108c5a6c  6701d0e5  ldrb r0, [r0,#0x167]
108c5a70  020010e3  tst r0, #0x2
108c5a74  2d00001a  bne 0x108c5b30
108c5a78  d4009fe5  ldr r0, [0x108c5b54]
108c5a7c  604090e5  ldr r4, [r0,#0x60]
108c5a80  000054e3  cmp r4, #0x0
108c5a84  2f00000a  beq 0x108c5b48
108c5a88  0c0094e5  ldr r0, [r4,#0xc]
108c5a8c  000050e3  cmp r0, #0x0
108c5a90  2900000a  beq 0x108c5b3c
108c5a94  1000d4e5  ldrb r0, [r4,#0x10]
108c5a98  040050e3  cmp r0, #0x4
108c5a9c  1500001a  bne 0x108c5af8
108c5aa0  3b60d4e5  ldrb r6, [r4,#0x3b]
108c5aa4  088097e5  ldr r8, [r7,#0x8]
108c5aa8  180056e3  cmp r6, #0x18
108c5aac  0300003a  bcc 0x108c5ac0
108c5ab0  0020a0e3  mov r2, #0x0
108c5ab4  0210a0e1  cpy r1, r2
108c5ab8  0200a0e1  cpy r0, r2
108c5abc  34eafceb  bl 0x10800394
108c5ac0  066188e0  add r6, r8, r6, lsl #0x2
108c5ac4  840096e5  ldr r0, [r6,#0x84]
108c5ac8  000050e3  cmp r0, #0x0
108c5acc  0300001a  bne 0x108c5ae0
108c5ad0  0020a0e3  mov r2, #0x0
108c5ad4  0210a0e1  cpy r1, r2
108c5ad8  0200a0e1  cpy r0, r2
108c5adc  2ceafceb  bl 0x10800394
108c5ae0  840096e5  ldr r0, [r6,#0x84]
108c5ae4  000050e3  cmp r0, #0x0
108c5ae8  0200000a  beq 0x108c5af8
108c5aec  5c0090e5  ldr r0, [r0,#0x5c]
108c5af0  070010e3  tst r0, #0x7
108c5af4  1000001a  bne 0x108c5b3c
108c5af8  180094e5  ldr r0, [r4,#0x18]
108c5afc  010110e3  tst r0, #0x40000000
108c5b00  0d00001a  bne 0x108c5b3c
108c5b04  3a10d5e5  ldrb r1, [r5,#0x3a]
108c5b08  b803d5e1  ldrh r0, [r5,#0x38]
108c5b0c  0930a0e1  cpy r3, r9
108c5b10  f000cde1  strd r0, r1, [sp,#0x0]
108c5b14  0cc094e5  ldr r12, [r4,#0xc]
108c5b18  1020d5e5  ldrb r2, [r5,#0x10]
108c5b1c  0710a0e1  cpy r1, r7
108c5b20  0400a0e1  cpy r0, r4
108c5b24  3cff2fe1  blx r12
108c5b28  000050e3  cmp r0, #0x0
108c5b2c  0200000a  beq 0x108c5b3c
108c5b30  0cd08de2  add sp, sp, #0xc
108c5b34  0100a0e3  mov r0, #0x1
108c5b38  f083bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,pc}
108c5b3c  044094e5  ldr r4, [r4,#0x4]
108c5b40  000054e3  cmp r4, #0x0
108c5b44  cfffff1a  bne 0x108c5a88
108c5b48  0cd08de2  add sp, sp, #0xc
108c5b4c  0000a0e3  mov r0, #0x0
108c5b50  f083bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,pc}
