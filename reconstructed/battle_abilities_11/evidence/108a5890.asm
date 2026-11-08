108a5890  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108a5894  0280a0e1  cpy r8, r2
108a5898  00a0a0e1  cpy r10, r0
108a589c  0190a0e1  cpy r9, r1
108a58a0  0200a0e3  mov r0, #0x2
108a58a4  1c18ffeb  bl 0x1086b91c
108a58a8  ff7000e2  and r7, r0, #0xff
108a58ac  0710a0e1  cpy r1, r7
108a58b0  0900a0e1  cpy r0, r9
108a58b4  2736ffeb  bl 0x10873158
108a58b8  080050e1  cmp r0, r8
108a58bc  7900001a  bne 0x108a5aa8
108a58c0  0710a0e1  cpy r1, r7
108a58c4  0900a0e1  cpy r0, r9
108a58c8  f4bafeeb  bl 0x108544a0
108a58cc  00b0a0e1  cpy r11, r0
108a58d0  0060a0e3  mov r6, #0x0
108a58d4  8f9efeeb  bl 0x1084d318
108a58d8  000050e3  cmp r0, #0x0
108a58dc  0160a003  moveq r6, #0x1
108a58e0  0b00a0e1  cpy r0, r11
108a58e4  f01fffeb  bl 0x1086d8ac
108a58e8  000050e3  cmp r0, #0x0
108a58ec  0160a013  movne r6, #0x1
108a58f0  0b00a0e1  cpy r0, r11
108a58f4  5201d0e5  ldrb r0, [r0,#0x152]
108a58f8  005050e2  subs r5, r0, #0x0
108a58fc  0040a0e3  mov r4, #0x0
108a5900  0700009a  bls 0x108a5924
108a5904  ff1004e2  and r1, r4, #0xff
108a5908  0b00a0e1  cpy r0, r11
108a590c  1c9efeeb  bl 0x1084d184
108a5910  000050e3  cmp r0, #0x0
108a5914  0160a013  movne r6, #0x1
108a5918  014084e2  add r4, r4, #0x1
108a591c  050054e1  cmp r4, r5
108a5920  f7ffff3a  bcc 0x108a5904
108a5924  000056e3  cmp r6, #0x0
108a5928  5500000a  beq 0x108a5a84
108a592c  0720a0e1  cpy r2, r7
108a5930  3810a0e3  mov r1, #0x38
108a5934  0900a0e1  cpy r0, r9
108a5938  8f11ffeb  bl 0x10869f7c
108a593c  0040a0e1  cpy r4, r0
108a5940  af1fa0e3  mov r1, #0x2bc
108a5944  b410c4e1  strh r1, [r4,#0x4]
108a5948  0150a0e3  mov r5, #0x1
108a594c  0a00a0e3  mov r0, #0xa
108a5950  0b50c4e5  strb r5, [r4,#0xb]
108a5954  0700c4e5  strb r0, [r4,#0x7]
108a5958  ab2f80e2  add r2, r0, #0x2ac
108a595c  0210a0e3  mov r1, #0x2
108a5960  0c0084e2  add r0, r4, #0xc
108a5964  0680c4e5  strb r8, [r4,#0x6]
108a5968  599efeeb  bl 0x1084d2d4
108a596c  0710a0e1  cpy r1, r7
108a5970  0c0084e2  add r0, r4, #0xc
108a5974  489efeeb  bl 0x1084d29c
108a5978  0410a0e1  cpy r1, r4
108a597c  0900a0e1  cpy r0, r9
108a5980  7008ffeb  bl 0x10867b48
108a5984  0b00a0e1  cpy r0, r11
108a5988  00f020e3  nop
108a598c  619efeeb  bl 0x1084d318
108a5990  000050e3  cmp r0, #0x0
108a5994  00f020e3  nop
108a5998  1100001a  bne 0x108a59e4
108a599c  0720a0e1  cpy r2, r7
108a59a0  0410a0e3  mov r1, #0x4
108a59a4  0900a0e1  cpy r0, r9
108a59a8  7311ffeb  bl 0x10869f7c
108a59ac  0040a0e1  cpy r4, r0
108a59b0  0670c0e5  strb r7, [r0,#0x6]
108a59b4  0e10a0e3  mov r1, #0xe
108a59b8  0b00a0e1  cpy r0, r11
108a59bc  973affeb  bl 0x10874420
108a59c0  0060a0e1  cpy r6, r0
108a59c4  0d10a0e3  mov r1, #0xd
108a59c8  0b00a0e1  cpy r0, r11
108a59cc  933affeb  bl 0x10874420
108a59d0  001046e0  sub r1, r6, r0
108a59d4  b410c4e1  strh r1, [r4,#0x4]
108a59d8  0410a0e1  cpy r1, r4
108a59dc  0900a0e1  cpy r0, r9
108a59e0  5808ffeb  bl 0x10867b48
108a59e4  0b00a0e1  cpy r0, r11
108a59e8  af1fffeb  bl 0x1086d8ac
108a59ec  000050e3  cmp r0, #0x0
108a59f0  00f020e3  nop
108a59f4  0a00000a  beq 0x108a5a24
108a59f8  0720a0e1  cpy r2, r7
108a59fc  0a10a0e3  mov r1, #0xa
108a5a00  0900a0e1  cpy r0, r9
108a5a04  5c11ffeb  bl 0x10869f7c
108a5a08  0010a0e1  cpy r1, r0
108a5a0c  2600a0e3  mov r0, #0x26
108a5a10  1150c1e5  strb r5, [r1,#0x11]
108a5a14  0570c1e5  strb r7, [r1,#0x5]
108a5a18  0400c1e5  strb r0, [r1,#0x4]
108a5a1c  0900a0e1  cpy r0, r9
108a5a20  4808ffeb  bl 0x10867b48
108a5a24  0b00a0e1  cpy r0, r11
108a5a28  5201d0e5  ldrb r0, [r0,#0x152]
108a5a2c  006050e2  subs r6, r0, #0x0
108a5a30  0040a0e3  mov r4, #0x0
108a5a34  1200009a  bls 0x108a5a84
108a5a38  ff1004e2  and r1, r4, #0xff
108a5a3c  0b00a0e1  cpy r0, r11
108a5a40  cf9dfeeb  bl 0x1084d184
108a5a44  0050b0e1  movs r5, r0
108a5a48  00f020e3  nop
108a5a4c  0900000a  beq 0x108a5a78
108a5a50  0720a0e1  cpy r2, r7
108a5a54  0810a0e3  mov r1, #0x8
108a5a58  0900a0e1  cpy r0, r9
108a5a5c  4611ffeb  bl 0x10869f7c
108a5a60  0570c0e5  strb r7, [r0,#0x5]
108a5a64  0640c0e5  strb r4, [r0,#0x6]
108a5a68  0010a0e1  cpy r1, r0
108a5a6c  0450c0e5  strb r5, [r0,#0x4]
108a5a70  0900a0e1  cpy r0, r9
108a5a74  3308ffeb  bl 0x10867b48
108a5a78  014084e2  add r4, r4, #0x1
108a5a7c  060054e1  cmp r4, r6
108a5a80  ecffff3a  bcc 0x108a5a38
108a5a84  0a00a0e1  cpy r0, r10
108a5a88  b803d0e1  ldrh r0, [r0,#0x38]
108a5a8c  ff1000e2  and r1, r0, #0xff
108a5a90  0820a0e1  cpy r2, r8
108a5a94  0900a0e1  cpy r0, r9
108a5a98  de19ffeb  bl 0x1086c218
108a5a9c  0a00a0e1  cpy r0, r10
108a5aa0  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108a5aa4  c89dfeea  b 0x1084d1cc
108a5aa8  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
