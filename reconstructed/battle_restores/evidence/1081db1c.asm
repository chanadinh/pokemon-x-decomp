1081db1c  f04f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1081db20  0cd04de2  sub sp, sp, #0xc
1081db24  0050a0e1  cpy r5, r0
1081db28  0140a0e1  cpy r4, r1
1081db2c  0460d1e5  ldrb r6, [r1,#0x4]
1081db30  087090e5  ldr r7, [r0,#0x8]
1081db34  180056e3  cmp r6, #0x18
1081db38  0300003a  bcc 0x1081db4c
1081db3c  0020a0e3  mov r2, #0x0
1081db40  0210a0e1  cpy r1, r2
1081db44  0200a0e1  cpy r0, r2
1081db48  118affeb  bl 0x10800394
1081db4c  066187e0  add r6, r7, r6, lsl #0x2
1081db50  840096e5  ldr r0, [r6,#0x84]
1081db54  000050e3  cmp r0, #0x0
1081db58  0300001a  bne 0x1081db6c
1081db5c  0020a0e3  mov r2, #0x0
1081db60  0210a0e1  cpy r1, r2
1081db64  0200a0e1  cpy r0, r2
1081db68  098affeb  bl 0x10800394
1081db6c  846096e5  ldr r6, [r6,#0x84]
1081db70  0d10a0e3  mov r1, #0xd
1081db74  0600a0e1  cpy r0, r6
1081db78  285a01eb  bl 0x10874420
1081db7c  000050e3  cmp r0, #0x0
1081db80  00f020e3  nop
1081db84  5f00000a  beq 0x1081dd08
1081db88  b610d4e1  ldrh r1, [r4,#0x6]
1081db8c  0600a0e1  cpy r0, r6
1081db90  e43802eb  bl 0x108abf28
1081db94  000050e3  cmp r0, #0x0
1081db98  04008de5  str r0, [sp,#0x4]
1081db9c  5900000a  beq 0x1081dd08
1081dba0  0070a0e3  mov r7, #0x0
1081dba4  0c0095e5  ldr r0, [r5,#0xc]
1081dba8  4910a0e3  mov r1, #0x49
1081dbac  0730a0e1  cpy r3, r7
1081dbb0  6e20a0e3  mov r2, #0x6e
1081dbb4  a41801eb  bl 0x10863e4c
1081dbb8  08008de5  str r0, [sp,#0x8]
1081dbbc  010985e2  add r0, r5, #0x4000
1081dbc0  df0f80e2  add r0, r0, #0x37c
1081dbc4  0080a0e1  cpy r8, r0
1081dbc8  ad3501eb  bl 0x1086b284
1081dbcc  0090a0e1  cpy r9, r0
1081dbd0  000094e5  ldr r0, [r4,#0x0]
1081dbd4  8009a0e1  mov r0, r0, lsl #0x13
1081dbd8  a0ada0e1  mov r10, r0, lsr #0x1b
1081dbdc  e83701eb  bl 0x1086bb84
1081dbe0  1910d6e5  ldrb r1, [r6,#0x19]
1081dbe4  0200a0e3  mov r0, #0x2
1081dbe8  4c3601eb  bl 0x1086b520
1081dbec  0a10a0e1  cpy r1, r10
1081dbf0  0300a0e3  mov r0, #0x3
1081dbf4  493601eb  bl 0x1086b520
1081dbf8  14a19fe5  ldr r10, [0x1081dd14]
1081dbfc  7c10a0e3  mov r1, #0x7c
1081dc00  0500a0e1  cpy r0, r5
1081dc04  0720a0e3  mov r2, #0x7
1081dc08  68c09ae5  ldr r12, [r10,#0x68]
1081dc0c  0030a0e3  mov r3, #0x0
1081dc10  01c08ce2  add r12, r12, #0x1
1081dc14  68c08ae5  str r12, [r10,#0x68]
1081dc18  9e1c01eb  bl 0x10864e98
1081dc1c  68009ae5  ldr r0, [r10,#0x68]
1081dc20  00b0a0e3  mov r11, #0x0
1081dc24  010050e2  subs r0, r0, #0x1
1081dc28  68008ae5  str r0, [r10,#0x68]
1081dc2c  0600001a  bne 0x1081dc4c
1081dc30  60009ae5  ldr r0, [r10,#0x60]
1081dc34  000050e3  cmp r0, #0x0
1081dc38  0300000a  beq 0x1081dc4c
1081dc3c  b8b1c0e1  strh r11, [r0,#0x18]
1081dc40  040090e5  ldr r0, [r0,#0x4]
1081dc44  000050e3  cmp r0, #0x0
1081dc48  fbffff1a  bne 0x1081dc3c
1081dc4c  00f020e3  nop
1081dc50  0a3701eb  bl 0x1086b880
1081dc54  bc009fe5  ldr r0, [0x1081dd18]
1081dc58  050090e7  ldr r0, [r0,r5]
1081dc5c  010110e3  tst r0, #0x40000000
1081dc60  0b00000a  beq 0x1081dc94
1081dc64  020200e2  and r0, r0, #0x20000000
1081dc68  a00eb0e1  movs r0, r0, lsr #0x1d
1081dc6c  0800000a  beq 0x1081dc94
1081dc70  b600d4e1  ldrh r0, [r4,#0x6]
1081dc74  0170a0e3  mov r7, #0x1
1081dc78  c98affeb  bl 0x108007a4
1081dc7c  00008de5  str r0, [sp,#0x0]
1081dc80  0430d4e5  ldrb r3, [r4,#0x4]
1081dc84  0c0095e5  ldr r0, [r5,#0xc]
1081dc88  08109de5  ldr r1, [sp,#0x8]
1081dc8c  4920a0e3  mov r2, #0x49
1081dc90  481f01eb  bl 0x108659b8
1081dc94  009088e5  str r9, [r8,#0x0]
1081dc98  04009de5  ldr r0, [sp,#0x4]
1081dc9c  cc1f02eb  bl 0x108a5bd4
1081dca0  0500d4e5  ldrb r0, [r4,#0x5]
1081dca4  000050e3  cmp r0, #0x0
1081dca8  0c00000a  beq 0x1081dce0
1081dcac  b600d4e1  ldrh r0, [r4,#0x6]
1081dcb0  bb8affeb  bl 0x108007a4
1081dcb4  000050e3  cmp r0, #0x0
1081dcb8  00f020e3  nop
1081dcbc  0700000a  beq 0x1081dce0
1081dcc0  6c11d6e5  ldrb r1, [r6,#0x16c]
1081dcc4  0b30a0e1  cpy r3, r11
1081dcc8  011081e3  orr r1, r1, #0x1
1081dccc  6c11c6e5  strb r1, [r6,#0x16c]
1081dcd0  0c0095e5  ldr r0, [r5,#0xc]
1081dcd4  1920d6e5  ldrb r2, [r6,#0x19]
1081dcd8  1f10a0e3  mov r1, #0x1f
1081dcdc  523901eb  bl 0x1086c22c
1081dce0  000057e3  cmp r7, #0x0
1081dce4  0400000a  beq 0x1081dcfc
1081dce8  b620d4e1  ldrh r2, [r4,#0x6]
1081dcec  0130a0e3  mov r3, #0x1
1081dcf0  0610a0e1  cpy r1, r6
1081dcf4  0500a0e1  cpy r0, r5
1081dcf8  e63401eb  bl 0x1086b098
1081dcfc  0cd08de2  add sp, sp, #0xc
1081dd00  0100a0e3  mov r0, #0x1
1081dd04  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
1081dd08  0cd08de2  add sp, sp, #0xc
1081dd0c  0000a0e3  mov r0, #0x0
1081dd10  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
