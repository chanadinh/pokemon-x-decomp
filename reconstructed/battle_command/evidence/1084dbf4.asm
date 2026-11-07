1084dbf4  f04f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084dbf8  0cd04de2  sub sp, sp, #0xc
1084dbfc  0050a0e1  cpy r5, r0
1084dc00  0270a0e1  cpy r7, r2
1084dc04  0340a0e1  cpy r4, r3
1084dc08  1a5100eb  bl 0x10862078
1084dc0c  f09b9fe5  ldr r9, [0x1084e804]
1084dc10  220057e3  cmp r7, #0x22
1084dc14  0610a0e3  mov r1, #0x6
1084dc18  0580a0e3  mov r8, #0x5
1084dc1c  086085e2  add r6, r5, #0x8
1084dc20  8900000a  beq 0x1084de4c
1084dc24  200000aa  bge 0x1084dcac
1084dc28  090057e3  cmp r7, #0x9
1084dc2c  0070a003  moveq r7, #0x0
1084dc30  e102000a  beq 0x1084e7bc
1084dc34  0b0000aa  bge 0x1084dc68
1084dc38  090057e3  cmp r7, #0x9
1084dc3c  07f19f37  ldrcc pc, [pc,r7,lsl #0x2]
1084dc40  3e0000ea  b 0x1084dd40
1084dc68  140057e3  cmp r7, #0x14
1084dc6c  1001000a  beq 0x1084e0b4
1084dc70  060000aa  bge 0x1084dc90
1084dc74  110057e3  cmp r7, #0x11
1084dc78  4200000a  beq 0x1084dd88
1084dc7c  120057e3  cmp r7, #0x12
1084dc80  5e00000a  beq 0x1084de00
1084dc84  130057e3  cmp r7, #0x13
1084dc88  2c00001a  bne 0x1084dd40
1084dc8c  a80000ea  b 0x1084df34
1084dc90  150057e3  cmp r7, #0x15
1084dc94  a601000a  beq 0x1084e334
1084dc98  160057e3  cmp r7, #0x16
1084dc9c  1d02000a  beq 0x1084e518
1084dca0  210057e3  cmp r7, #0x21
1084dca4  2500001a  bne 0x1084dd40
1084dca8  3c0000ea  b 0x1084dda0
1084dcac  340057e3  cmp r7, #0x34
1084dcb0  0e00a0e3  mov r0, #0xe
1084dcb4  4901000a  beq 0x1084e1e0
1084dcb8  100000ca  bgt 0x1084dd00
1084dcbc  260057e3  cmp r7, #0x26
1084dcc0  5202000a  beq 0x1084e610
1084dcc4  060000ca  bgt 0x1084dce4
1084dcc8  230057e3  cmp r7, #0x23
1084dccc  a700000a  beq 0x1084df70
1084dcd0  240057e3  cmp r7, #0x24
1084dcd4  3201000a  beq 0x1084e1a4
1084dcd8  250057e3  cmp r7, #0x25
1084dcdc  1700001a  bne 0x1084dd40
1084dce0  a90100ea  b 0x1084e38c
1084dce4  300057e3  cmp r7, #0x30
1084dce8  f101000a  beq 0x1084e4b4
1084dcec  320057e3  cmp r7, #0x32
1084dcf0  6700000a  beq 0x1084de94
1084dcf4  330057e3  cmp r7, #0x33
1084dcf8  1000001a  bne 0x1084dd40
1084dcfc  a60000ea  b 0x1084df9c
1084dd00  430057e3  cmp r7, #0x43
1084dd04  af00000a  beq 0x1084dfc8
1084dd08  060000aa  bge 0x1084dd28
1084dd0c  350057e3  cmp r7, #0x35
1084dd10  d101000a  beq 0x1084e45c
1084dd14  360057e3  cmp r7, #0x36
1084dd18  6a02000a  beq 0x1084e6c8
1084dd1c  420057e3  cmp r7, #0x42
1084dd20  0600001a  bne 0x1084dd40
1084dd24  630000ea  b 0x1084deb8
1084dd28  440057e3  cmp r7, #0x44
1084dd2c  3a01000a  beq 0x1084e21c
1084dd30  530057e3  cmp r7, #0x53
1084dd34  bf00000a  beq 0x1084e038
1084dd38  540057e3  cmp r7, #0x54
1084dd3c  4501000a  beq 0x1084e258
1084dd40  0cd08de2  add sp, sp, #0xc
1084dd44  0020a0e3  mov r2, #0x0
1084dd48  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084dd4c  0210a0e1  cpy r1, r2
1084dd50  0200a0e1  cpy r0, r2
1084dd54  8ec9feea  b 0x10800394
1084dd58  000094e5  ldr r0, [r4,#0x0]
1084dd5c  001095e5  ldr r1, [r5,#0x0]
1084dd60  ff4000e2  and r4, r0, #0xff
1084dd64  090051e1  cmp r1, r9
1084dd68  cd01003a  bcc 0x1084e4a4
1084dd6c  0020a0e3  mov r2, #0x0
1084dd70  0210a0e1  cpy r1, r2
1084dd74  0200a0e1  cpy r0, r2
1084dd78  89c9feeb  bl 0x108003a4
1084dd7c  00f020e3  nop
1084dd80  00f020e3  nop
1084dd84  c60100ea  b 0x1084e4a4
1084dd88  000094e5  ldr r0, [r4,#0x0]
1084dd8c  0cd08de2  add sp, sp, #0xc
1084dd90  7010ffe6  uxth r1, r0
1084dd94  0500a0e1  cpy r0, r5
1084dd98  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084dd9c  b55000ea  b 0x10862078
1084dda0  001094e5  ldr r1, [r4,#0x0]
1084dda4  0cd08de2  add sp, sp, #0xc
1084dda8  0500a0e1  cpy r0, r5
1084ddac  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084ddb0  6effffea  b 0x1084db70
1084ddb4  000094e5  ldr r0, [r4,#0x0]
1084ddb8  001095e5  ldr r1, [r5,#0x0]
1084ddbc  ff7000e2  and r7, r0, #0xff
1084ddc0  090051e1  cmp r1, r9
1084ddc4  0300003a  bcc 0x1084ddd8
1084ddc8  0020a0e3  mov r2, #0x0
1084ddcc  0210a0e1  cpy r1, r2
1084ddd0  0200a0e1  cpy r0, r2
1084ddd4  72c9feeb  bl 0x108003a4
1084ddd8  000095e5  ldr r0, [r5,#0x0]
1084dddc  012080e2  add r2, r0, #0x1
1084dde0  002085e5  str r2, [r5,#0x0]
1084dde4  0670c0e7  strb r7, [r0,r6]
1084dde8  040094e5  ldr r0, [r4,#0x4]
1084ddec  001095e5  ldr r1, [r5,#0x0]
1084ddf0  ff4000e2  and r4, r0, #0xff
1084ddf4  090051e1  cmp r1, r9
1084ddf8  a901003a  bcc 0x1084e4a4
1084ddfc  daffffea  b 0x1084dd6c
1084de00  000094e5  ldr r0, [r4,#0x0]
1084de04  001095e5  ldr r1, [r5,#0x0]
1084de08  ff7000e2  and r7, r0, #0xff
1084de0c  090051e1  cmp r1, r9
1084de10  0300003a  bcc 0x1084de24
1084de14  0020a0e3  mov r2, #0x0
1084de18  0210a0e1  cpy r1, r2
1084de1c  0200a0e1  cpy r0, r2
1084de20  5fc9feeb  bl 0x108003a4
1084de24  000095e5  ldr r0, [r5,#0x0]
1084de28  012080e2  add r2, r0, #0x1
1084de2c  002085e5  str r2, [r5,#0x0]
1084de30  0670c0e7  strb r7, [r0,r6]
1084de34  040094e5  ldr r0, [r4,#0x4]
1084de38  0cd08de2  add sp, sp, #0xc
1084de3c  7010ffe6  uxth r1, r0
1084de40  0500a0e1  cpy r0, r5
1084de44  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084de48  8a5000ea  b 0x10862078
1084de4c  000094e5  ldr r0, [r4,#0x0]
1084de50  001095e5  ldr r1, [r5,#0x0]
1084de54  ff7000e2  and r7, r0, #0xff
1084de58  090051e1  cmp r1, r9
1084de5c  0300003a  bcc 0x1084de70
1084de60  0020a0e3  mov r2, #0x0
1084de64  0210a0e1  cpy r1, r2
1084de68  0200a0e1  cpy r0, r2
1084de6c  4cc9feeb  bl 0x108003a4
1084de70  000095e5  ldr r0, [r5,#0x0]
1084de74  012080e2  add r2, r0, #0x1
1084de78  002085e5  str r2, [r5,#0x0]
1084de7c  0670c0e7  strb r7, [r0,r6]
1084de80  041094e5  ldr r1, [r4,#0x4]
1084de84  0cd08de2  add sp, sp, #0xc
1084de88  0500a0e1  cpy r0, r5
1084de8c  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084de90  36ffffea  b 0x1084db70
1084de94  0430a0e3  mov r3, #0x4
1084de98  0320a0e1  cpy r2, r3
1084de9c  d000c4e1  ldrd r0, r1, [r4,#0x0]
1084dea0  02ffffeb  bl 0x1084dab0
1084dea4  0040a0e1  cpy r4, r0
1084dea8  000095e5  ldr r0, [r5,#0x0]
1084deac  090050e1  cmp r0, r9
1084deb0  7b01003a  bcc 0x1084e4a4
1084deb4  acffffea  b 0x1084dd6c
1084deb8  0330a0e3  mov r3, #0x3
1084debc  0520a0e3  mov r2, #0x5
1084dec0  d000c4e1  ldrd r0, r1, [r4,#0x0]
1084dec4  f9feffeb  bl 0x1084dab0
1084dec8  0040a0e1  cpy r4, r0
1084decc  000095e5  ldr r0, [r5,#0x0]
1084ded0  090050e1  cmp r0, r9
1084ded4  7201003a  bcc 0x1084e4a4
1084ded8  a3ffffea  b 0x1084dd6c
1084dedc  0330a0e3  mov r3, #0x3
1084dee0  0520a0e3  mov r2, #0x5
1084dee4  d000c4e1  ldrd r0, r1, [r4,#0x0]
1084dee8  f0feffeb  bl 0x1084dab0
1084deec  0070a0e1  cpy r7, r0
1084def0  000095e5  ldr r0, [r5,#0x0]
1084def4  090050e1  cmp r0, r9
1084def8  0300003a  bcc 0x1084df0c
1084defc  0020a0e3  mov r2, #0x0
1084df00  0210a0e1  cpy r1, r2
1084df04  0200a0e1  cpy r0, r2
1084df08  25c9feeb  bl 0x108003a4
1084df0c  000095e5  ldr r0, [r5,#0x0]
1084df10  012080e2  add r2, r0, #0x1
1084df14  002085e5  str r2, [r5,#0x0]
1084df18  0670c0e7  strb r7, [r0,r6]
1084df1c  080094e5  ldr r0, [r4,#0x8]
1084df20  001095e5  ldr r1, [r5,#0x0]
1084df24  ff4000e2  and r4, r0, #0xff
1084df28  090051e1  cmp r1, r9
1084df2c  5c01003a  bcc 0x1084e4a4
1084df30  8dffffea  b 0x1084dd6c
1084df34  0330a0e3  mov r3, #0x3
1084df38  0520a0e3  mov r2, #0x5
1084df3c  d000c4e1  ldrd r0, r1, [r4,#0x0]
1084df40  dafeffeb  bl 0x1084dab0
1084df44  0070a0e1  cpy r7, r0
1084df48  000095e5  ldr r0, [r5,#0x0]
1084df4c  090050e1  cmp r0, r9
1084df50  2e00003a  bcc 0x1084e010
1084df54  0020a0e3  mov r2, #0x0
1084df58  0210a0e1  cpy r1, r2
1084df5c  0200a0e1  cpy r0, r2
1084df60  0fc9feeb  bl 0x108003a4
1084df64  00f020e3  nop
1084df68  00f020e3  nop
1084df6c  270000ea  b 0x1084e010
1084df70  00808de5  str r8, [sp,#0x0]
1084df74  04808de5  str r8, [sp,#0x4]
1084df78  08808de5  str r8, [sp,#0x8]
1084df7c  0e0094e8  ldmia r4,{r1,r2,r3}
1084df80  0200a0e3  mov r0, #0x2
1084df84  85feffeb  bl 0x1084d9a0
1084df88  7010ffe6  uxth r1, r0
1084df8c  0cd08de2  add sp, sp, #0xc
1084df90  0500a0e1  cpy r0, r5
1084df94  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084df98  365000ea  b 0x10862078
1084df9c  00808de5  str r8, [sp,#0x0]
1084dfa0  08008de5  str r0, [sp,#0x8]
1084dfa4  04808de5  str r8, [sp,#0x4]
1084dfa8  0e0094e8  ldmia r4,{r1,r2,r3}
1084dfac  0300a0e3  mov r0, #0x3
1084dfb0  7afeffeb  bl 0x1084d9a0
1084dfb4  0010a0e1  cpy r1, r0
1084dfb8  0cd08de2  add sp, sp, #0xc
1084dfbc  0500a0e1  cpy r0, r5
1084dfc0  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084dfc4  59feffea  b 0x1084d930
1084dfc8  000094e5  ldr r0, [r4,#0x0]
1084dfcc  001095e5  ldr r1, [r5,#0x0]
1084dfd0  ff7000e2  and r7, r0, #0xff
1084dfd4  090051e1  cmp r1, r9
1084dfd8  0300003a  bcc 0x1084dfec
1084dfdc  0020a0e3  mov r2, #0x0
1084dfe0  0210a0e1  cpy r1, r2
1084dfe4  0200a0e1  cpy r0, r2
1084dfe8  edc8feeb  bl 0x108003a4
1084dfec  000095e5  ldr r0, [r5,#0x0]
1084dff0  012080e2  add r2, r0, #0x1
1084dff4  002085e5  str r2, [r5,#0x0]
1084dff8  0670c0e7  strb r7, [r0,r6]
1084dffc  040094e5  ldr r0, [r4,#0x4]
1084e000  001095e5  ldr r1, [r5,#0x0]
1084e004  ff7000e2  and r7, r0, #0xff
1084e008  090051e1  cmp r1, r9
1084e00c  d0ffff2a  bcs 0x1084df54
1084e010  000095e5  ldr r0, [r5,#0x0]
1084e014  012080e2  add r2, r0, #0x1
1084e018  002085e5  str r2, [r5,#0x0]
1084e01c  0670c0e7  strb r7, [r0,r6]
1084e020  080094e5  ldr r0, [r4,#0x8]
1084e024  0cd08de2  add sp, sp, #0xc
1084e028  7010ffe6  uxth r1, r0
1084e02c  0500a0e1  cpy r0, r5
1084e030  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e034  0f5000ea  b 0x10862078
1084e038  000094e5  ldr r0, [r4,#0x0]
1084e03c  001095e5  ldr r1, [r5,#0x0]
1084e040  ff7000e2  and r7, r0, #0xff
1084e044  090051e1  cmp r1, r9
1084e048  0300003a  bcc 0x1084e05c
1084e04c  0020a0e3  mov r2, #0x0
1084e050  0210a0e1  cpy r1, r2
1084e054  0200a0e1  cpy r0, r2
1084e058  d1c8feeb  bl 0x108003a4
1084e05c  000095e5  ldr r0, [r5,#0x0]
1084e060  012080e2  add r2, r0, #0x1
1084e064  002085e5  str r2, [r5,#0x0]
1084e068  0670c0e7  strb r7, [r0,r6]
1084e06c  040094e5  ldr r0, [r4,#0x4]
1084e070  001095e5  ldr r1, [r5,#0x0]
1084e074  ff7000e2  and r7, r0, #0xff
1084e078  090051e1  cmp r1, r9
1084e07c  0300003a  bcc 0x1084e090
1084e080  0020a0e3  mov r2, #0x0
1084e084  0210a0e1  cpy r1, r2
1084e088  0200a0e1  cpy r0, r2
1084e08c  c4c8feeb  bl 0x108003a4
1084e090  000095e5  ldr r0, [r5,#0x0]
1084e094  012080e2  add r2, r0, #0x1
1084e098  002085e5  str r2, [r5,#0x0]
1084e09c  0670c0e7  strb r7, [r0,r6]
1084e0a0  081094e5  ldr r1, [r4,#0x8]
1084e0a4  0cd08de2  add sp, sp, #0xc
1084e0a8  0500a0e1  cpy r0, r5
1084e0ac  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e0b0  aefeffea  b 0x1084db70
1084e0b4  0330a0e3  mov r3, #0x3
1084e0b8  0520a0e3  mov r2, #0x5
1084e0bc  d000c4e1  ldrd r0, r1, [r4,#0x0]
1084e0c0  7afeffeb  bl 0x1084dab0
1084e0c4  0070a0e1  cpy r7, r0
1084e0c8  000095e5  ldr r0, [r5,#0x0]
1084e0cc  090050e1  cmp r0, r9
1084e0d0  0300003a  bcc 0x1084e0e4
1084e0d4  0020a0e3  mov r2, #0x0
1084e0d8  0210a0e1  cpy r1, r2
1084e0dc  0200a0e1  cpy r0, r2
1084e0e0  afc8feeb  bl 0x108003a4
1084e0e4  000095e5  ldr r0, [r5,#0x0]
1084e0e8  0330a0e3  mov r3, #0x3
1084e0ec  012080e2  add r2, r0, #0x1
1084e0f0  002085e5  str r2, [r5,#0x0]
1084e0f4  0670c0e7  strb r7, [r0,r6]
1084e0f8  0520a0e3  mov r2, #0x5
1084e0fc  d800c4e1  ldrd r0, r1, [r4,#0x8]
1084e100  6afeffeb  bl 0x1084dab0
1084e104  0040a0e1  cpy r4, r0
1084e108  000095e5  ldr r0, [r5,#0x0]
1084e10c  090050e1  cmp r0, r9
1084e110  e300003a  bcc 0x1084e4a4
1084e114  14ffffea  b 0x1084dd6c
1084e118  0330a0e3  mov r3, #0x3
1084e11c  0520a0e3  mov r2, #0x5
1084e120  d000c4e1  ldrd r0, r1, [r4,#0x0]
1084e124  61feffeb  bl 0x1084dab0
1084e128  0070a0e1  cpy r7, r0
1084e12c  000095e5  ldr r0, [r5,#0x0]
1084e130  090050e1  cmp r0, r9
1084e134  0300003a  bcc 0x1084e148
1084e138  0020a0e3  mov r2, #0x0
1084e13c  0210a0e1  cpy r1, r2
1084e140  0200a0e1  cpy r0, r2
1084e144  96c8feeb  bl 0x108003a4
1084e148  000095e5  ldr r0, [r5,#0x0]
1084e14c  012080e2  add r2, r0, #0x1
1084e150  002085e5  str r2, [r5,#0x0]
1084e154  0670c0e7  strb r7, [r0,r6]
1084e158  080094e5  ldr r0, [r4,#0x8]
1084e15c  001095e5  ldr r1, [r5,#0x0]
1084e160  ff7000e2  and r7, r0, #0xff
1084e164  090051e1  cmp r1, r9
1084e168  0300003a  bcc 0x1084e17c
1084e16c  0020a0e3  mov r2, #0x0
1084e170  0210a0e1  cpy r1, r2
1084e174  0200a0e1  cpy r0, r2
1084e178  89c8feeb  bl 0x108003a4
1084e17c  000095e5  ldr r0, [r5,#0x0]
1084e180  012080e2  add r2, r0, #0x1
1084e184  002085e5  str r2, [r5,#0x0]
1084e188  0670c0e7  strb r7, [r0,r6]
1084e18c  0c0094e5  ldr r0, [r4,#0xc]
1084e190  0cd08de2  add sp, sp, #0xc
1084e194  7010ffe6  uxth r1, r0
1084e198  0500a0e1  cpy r0, r5
1084e19c  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e1a0  b44f00ea  b 0x10862078
1084e1a4  00808de5  str r8, [sp,#0x0]
1084e1a8  08008de5  str r0, [sp,#0x8]
1084e1ac  04808de5  str r8, [sp,#0x4]
1084e1b0  0e0094e8  ldmia r4,{r1,r2,r3}
1084e1b4  0300a0e3  mov r0, #0x3
1084e1b8  f8fdffeb  bl 0x1084d9a0
1084e1bc  0010a0e1  cpy r1, r0
1084e1c0  0500a0e1  cpy r0, r5
1084e1c4  d9fdffeb  bl 0x1084d930
1084e1c8  0c0094e5  ldr r0, [r4,#0xc]
1084e1cc  001095e5  ldr r1, [r5,#0x0]
1084e1d0  ff4000e2  and r4, r0, #0xff
1084e1d4  090051e1  cmp r1, r9
1084e1d8  b100003a  bcc 0x1084e4a4
1084e1dc  e2feffea  b 0x1084dd6c
1084e1e0  00808de5  str r8, [sp,#0x0]
1084e1e4  08008de5  str r0, [sp,#0x8]
1084e1e8  04808de5  str r8, [sp,#0x4]
1084e1ec  0e0094e8  ldmia r4,{r1,r2,r3}
1084e1f0  0300a0e3  mov r0, #0x3
1084e1f4  e9fdffeb  bl 0x1084d9a0
1084e1f8  0010a0e1  cpy r1, r0
1084e1fc  0500a0e1  cpy r0, r5
1084e200  cafdffeb  bl 0x1084d930
1084e204  0c0094e5  ldr r0, [r4,#0xc]
1084e208  0cd08de2  add sp, sp, #0xc
1084e20c  7010ffe6  uxth r1, r0
1084e210  0500a0e1  cpy r0, r5
1084e214  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e218  964f00ea  b 0x10862078
1084e21c  00808de5  str r8, [sp,#0x0]
1084e220  08108de5  str r1, [sp,#0x8]
1084e224  04808de5  str r8, [sp,#0x4]
1084e228  0e0094e8  ldmia r4,{r1,r2,r3}
1084e22c  0200a0e3  mov r0, #0x2
1084e230  dafdffeb  bl 0x1084d9a0
1084e234  7010ffe6  uxth r1, r0
1084e238  0500a0e1  cpy r0, r5
1084e23c  8d4f00eb  bl 0x10862078
1084e240  0c0094e5  ldr r0, [r4,#0xc]
1084e244  0cd08de2  add sp, sp, #0xc
1084e248  7010ffe6  uxth r1, r0
1084e24c  0500a0e1  cpy r0, r5
1084e250  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e254  874f00ea  b 0x10862078
1084e258  000094e5  ldr r0, [r4,#0x0]
1084e25c  001095e5  ldr r1, [r5,#0x0]
1084e260  ff7000e2  and r7, r0, #0xff
1084e264  090051e1  cmp r1, r9
1084e268  0300003a  bcc 0x1084e27c
1084e26c  0020a0e3  mov r2, #0x0
1084e270  0210a0e1  cpy r1, r2
1084e274  0200a0e1  cpy r0, r2
1084e278  49c8feeb  bl 0x108003a4
1084e27c  000095e5  ldr r0, [r5,#0x0]
1084e280  012080e2  add r2, r0, #0x1
1084e284  002085e5  str r2, [r5,#0x0]
1084e288  0670c0e7  strb r7, [r0,r6]
1084e28c  040094e5  ldr r0, [r4,#0x4]
1084e290  001095e5  ldr r1, [r5,#0x0]
1084e294  ff7000e2  and r7, r0, #0xff
1084e298  090051e1  cmp r1, r9
1084e29c  0300003a  bcc 0x1084e2b0
1084e2a0  0020a0e3  mov r2, #0x0
1084e2a4  0210a0e1  cpy r1, r2
1084e2a8  0200a0e1  cpy r0, r2
1084e2ac  3cc8feeb  bl 0x108003a4
1084e2b0  000095e5  ldr r0, [r5,#0x0]
1084e2b4  012080e2  add r2, r0, #0x1
1084e2b8  002085e5  str r2, [r5,#0x0]
1084e2bc  0670c0e7  strb r7, [r0,r6]
1084e2c0  080094e5  ldr r0, [r4,#0x8]
1084e2c4  7010ffe6  uxth r1, r0
1084e2c8  0500a0e1  cpy r0, r5
1084e2cc  694f00eb  bl 0x10862078
1084e2d0  0c0094e5  ldr r0, [r4,#0xc]
1084e2d4  0cd08de2  add sp, sp, #0xc
1084e2d8  7010ffe6  uxth r1, r0
1084e2dc  0500a0e1  cpy r0, r5
1084e2e0  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e2e4  634f00ea  b 0x10862078
1084e2e8  00808de5  str r8, [sp,#0x0]
1084e2ec  04808de5  str r8, [sp,#0x4]
1084e2f0  08808de5  str r8, [sp,#0x8]
1084e2f4  0e0094e8  ldmia r4,{r1,r2,r3}
1084e2f8  0200a0e3  mov r0, #0x2
1084e2fc  a7fdffeb  bl 0x1084d9a0
1084e300  7010ffe6  uxth r1, r0
1084e304  0500a0e1  cpy r0, r5
1084e308  5a4f00eb  bl 0x10862078
1084e30c  0c0094e5  ldr r0, [r4,#0xc]
1084e310  7010ffe6  uxth r1, r0
1084e314  0500a0e1  cpy r0, r5
1084e318  564f00eb  bl 0x10862078
1084e31c  100094e5  ldr r0, [r4,#0x10]
1084e320  0cd08de2  add sp, sp, #0xc
1084e324  7010ffe6  uxth r1, r0
1084e328  0500a0e1  cpy r0, r5
1084e32c  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e330  504f00ea  b 0x10862078
1084e334  0330a0e3  mov r3, #0x3
1084e338  0520a0e3  mov r2, #0x5
1084e33c  d000c4e1  ldrd r0, r1, [r4,#0x0]
1084e340  dafdffeb  bl 0x1084dab0
1084e344  0070a0e1  cpy r7, r0
1084e348  0130a0e3  mov r3, #0x1
1084e34c  0720a0e3  mov r2, #0x7
1084e350  d800c4e1  ldrd r0, r1, [r4,#0x8]
1084e354  d5fdffeb  bl 0x1084dab0
1084e358  0060a0e1  cpy r6, r0
1084e35c  0710a0e1  cpy r1, r7
1084e360  0500a0e1  cpy r0, r5
1084e364  434f00eb  bl 0x10862078
1084e368  0610a0e1  cpy r1, r6
1084e36c  0500a0e1  cpy r0, r5
1084e370  404f00eb  bl 0x10862078
1084e374  100094e5  ldr r0, [r4,#0x10]
1084e378  0cd08de2  add sp, sp, #0xc
1084e37c  7010ffe6  uxth r1, r0
1084e380  0500a0e1  cpy r0, r5
1084e384  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e388  3a4f00ea  b 0x10862078
1084e38c  047084e2  add r7, r4, #0x4
1084e390  00b094e5  ldr r11, [r4,#0x0]
1084e394  800597e8  ldmia r7,{r7,r8,r10}
1084e398  07005be3  cmp r11, #0x7
1084e39c  030000da  ble 0x1084e3b0
1084e3a0  0020a0e3  mov r2, #0x0
1084e3a4  0210a0e1  cpy r1, r2
1084e3a8  0200a0e1  cpy r0, r2
1084e3ac  f8c7feeb  bl 0x10800394
1084e3b0  070057e3  cmp r7, #0x7
1084e3b4  030000da  ble 0x1084e3c8
1084e3b8  0020a0e3  mov r2, #0x0
1084e3bc  0210a0e1  cpy r1, r2
1084e3c0  0200a0e1  cpy r0, r2
1084e3c4  f2c7feeb  bl 0x10800394
1084e3c8  010058e3  cmp r8, #0x1
1084e3cc  030000da  ble 0x1084e3e0
1084e3d0  0020a0e3  mov r2, #0x0
1084e3d4  0210a0e1  cpy r1, r2
1084e3d8  0200a0e1  cpy r0, r2
1084e3dc  ecc7feeb  bl 0x10800394
1084e3e0  01005ae3  cmp r10, #0x1
1084e3e4  030000da  ble 0x1084e3f8
1084e3e8  0020a0e3  mov r2, #0x0
1084e3ec  0210a0e1  cpy r1, r2
1084e3f0  0200a0e1  cpy r0, r2
1084e3f4  e6c7feeb  bl 0x10800394
1084e3f8  8b0ea0e1  mov r0, r11, lsl #0x1d
1084e3fc  871ea0e1  mov r1, r7, lsl #0x1d
1084e400  200ca0e1  mov r0, r0, lsr #0x18
1084e404  882fa0e1  mov r2, r8, lsl #0x1f
1084e408  a10d80e1  orr r0, r0, r1, lsr #0x1b
1084e40c  01300ae2  and r3, r10, #0x1
1084e410  220f80e1  orr r0, r0, r2, lsr #0x1e
1084e414  037080e1  orr r7, r0, r3
1084e418  000095e5  ldr r0, [r5,#0x0]
1084e41c  090050e1  cmp r0, r9
1084e420  0300003a  bcc 0x1084e434
1084e424  0020a0e3  mov r2, #0x0
1084e428  0210a0e1  cpy r1, r2
1084e42c  0200a0e1  cpy r0, r2
1084e430  dbc7feeb  bl 0x108003a4
1084e434  000095e5  ldr r0, [r5,#0x0]
1084e438  012080e2  add r2, r0, #0x1
1084e43c  002085e5  str r2, [r5,#0x0]
1084e440  0670c0e7  strb r7, [r0,r6]
1084e444  100094e5  ldr r0, [r4,#0x10]
1084e448  0cd08de2  add sp, sp, #0xc
1084e44c  7010ffe6  uxth r1, r0
1084e450  0500a0e1  cpy r0, r5
1084e454  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e458  064f00ea  b 0x10862078
1084e45c  00808de5  str r8, [sp,#0x0]
1084e460  08008de5  str r0, [sp,#0x8]
1084e464  04808de5  str r8, [sp,#0x4]
1084e468  0e0094e8  ldmia r4,{r1,r2,r3}
1084e46c  0300a0e3  mov r0, #0x3
1084e470  4afdffeb  bl 0x1084d9a0
1084e474  0070a0e1  cpy r7, r0
1084e478  0130a0e3  mov r3, #0x1
1084e47c  0720a0e3  mov r2, #0x7
1084e480  dc00c4e1  ldrd r0, r1, [r4,#0xc]
1084e484  89fdffeb  bl 0x1084dab0
1084e488  0040a0e1  cpy r4, r0
1084e48c  0710a0e1  cpy r1, r7
1084e490  0500a0e1  cpy r0, r5
1084e494  25fdffeb  bl 0x1084d930
1084e498  000095e5  ldr r0, [r5,#0x0]
1084e49c  090050e1  cmp r0, r9
1084e4a0  31feff2a  bcs 0x1084dd6c
1084e4a4  000095e5  ldr r0, [r5,#0x0]
1084e4a8  011080e2  add r1, r0, #0x1
1084e4ac  001085e5  str r1, [r5,#0x0]
1084e4b0  0640c0e7  strb r4, [r0,r6]
1084e4b4  0cd08de2  add sp, sp, #0xc
1084e4b8  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
1084e4bc  00808de5  str r8, [sp,#0x0]
1084e4c0  04808de5  str r8, [sp,#0x4]
1084e4c4  08808de5  str r8, [sp,#0x8]
1084e4c8  0e0094e8  ldmia r4,{r1,r2,r3}
1084e4cc  0200a0e3  mov r0, #0x2
1084e4d0  32fdffeb  bl 0x1084d9a0
1084e4d4  0c4084e2  add r4, r4, #0xc
1084e4d8  00808de5  str r8, [sp,#0x0]
1084e4dc  04808de5  str r8, [sp,#0x4]
1084e4e0  08808de5  str r8, [sp,#0x8]
1084e4e4  7060ffe6  uxth r6, r0
1084e4e8  0e0094e8  ldmia r4,{r1,r2,r3}
1084e4ec  0200a0e3  mov r0, #0x2
1084e4f0  2afdffeb  bl 0x1084d9a0
1084e4f4  7040ffe6  uxth r4, r0
1084e4f8  0610a0e1  cpy r1, r6
1084e4fc  0500a0e1  cpy r0, r5
1084e500  dc4e00eb  bl 0x10862078
1084e504  0cd08de2  add sp, sp, #0xc
1084e508  0410a0e1  cpy r1, r4
1084e50c  0500a0e1  cpy r0, r5
1084e510  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e514  d74e00ea  b 0x10862078
1084e518  800d94e8  ldmia r4,{r7,r8,r10,r11}
1084e51c  100094e5  ldr r0, [r4,#0x10]
1084e520  070057e3  cmp r7, #0x7
1084e524  00008de5  str r0, [sp,#0x0]
1084e528  030000da  ble 0x1084e53c
1084e52c  0020a0e3  mov r2, #0x0
1084e530  0210a0e1  cpy r1, r2
1084e534  0200a0e1  cpy r0, r2
1084e538  95c7feeb  bl 0x10800394
1084e53c  030058e3  cmp r8, #0x3
1084e540  030000da  ble 0x1084e554
1084e544  0020a0e3  mov r2, #0x0
1084e548  0210a0e1  cpy r1, r2
1084e54c  0200a0e1  cpy r0, r2
1084e550  8fc7feeb  bl 0x10800394
1084e554  01005ae3  cmp r10, #0x1
1084e558  030000da  ble 0x1084e56c
1084e55c  0020a0e3  mov r2, #0x0
1084e560  0210a0e1  cpy r1, r2
1084e564  0200a0e1  cpy r0, r2
1084e568  89c7feeb  bl 0x10800394
1084e56c  01005be3  cmp r11, #0x1
1084e570  030000da  ble 0x1084e584
1084e574  0020a0e3  mov r2, #0x0
1084e578  0210a0e1  cpy r1, r2
1084e57c  0200a0e1  cpy r0, r2
1084e580  83c7feeb  bl 0x10800394
1084e584  00009de5  ldr r0, [sp,#0x0]
1084e588  010050e3  cmp r0, #0x1
1084e58c  030000da  ble 0x1084e5a0
1084e590  0020a0e3  mov r2, #0x0
1084e594  0210a0e1  cpy r1, r2
1084e598  0200a0e1  cpy r0, r2
1084e59c  7cc7feeb  bl 0x10800394
1084e5a0  870ea0e1  mov r0, r7, lsl #0x1d
1084e5a4  082fa0e1  mov r2, r8, lsl #0x1e
1084e5a8  200ca0e1  mov r0, r0, lsr #0x18
1084e5ac  a22d80e1  orr r2, r0, r2, lsr #0x1b
1084e5b0  00009de5  ldr r0, [sp,#0x0]
1084e5b4  8a1fa0e1  mov r1, r10, lsl #0x1f
1084e5b8  8b3fa0e1  mov r3, r11, lsl #0x1f
1084e5bc  a11e82e1  orr r1, r2, r1, lsr #0x1d
1084e5c0  010000e2  and r0, r0, #0x1
1084e5c4  231f81e1  orr r1, r1, r3, lsr #0x1e
1084e5c8  007081e1  orr r7, r1, r0
1084e5cc  000095e5  ldr r0, [r5,#0x0]
1084e5d0  090050e1  cmp r0, r9
1084e5d4  0300003a  bcc 0x1084e5e8
1084e5d8  0020a0e3  mov r2, #0x0
1084e5dc  0210a0e1  cpy r1, r2
1084e5e0  0200a0e1  cpy r0, r2
1084e5e4  6ec7feeb  bl 0x108003a4
1084e5e8  000095e5  ldr r0, [r5,#0x0]
1084e5ec  012080e2  add r2, r0, #0x1
1084e5f0  002085e5  str r2, [r5,#0x0]
1084e5f4  0670c0e7  strb r7, [r0,r6]
1084e5f8  140094e5  ldr r0, [r4,#0x14]
1084e5fc  0cd08de2  add sp, sp, #0xc
1084e600  7010ffe6  uxth r1, r0
1084e604  0500a0e1  cpy r0, r5
1084e608  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e60c  994e00ea  b 0x10862078
1084e610  c00394e8  ldmia r4,{r6,r7,r8,r9}
1084e614  1f0056e3  cmp r6, #0x1f
1084e618  030000da  ble 0x1084e62c
1084e61c  0020a0e3  mov r2, #0x0
1084e620  0210a0e1  cpy r1, r2
1084e624  0200a0e1  cpy r0, r2
1084e628  59c7feeb  bl 0x10800394
1084e62c  1f0057e3  cmp r7, #0x1f
1084e630  030000da  ble 0x1084e644
1084e634  0020a0e3  mov r2, #0x0
1084e638  0210a0e1  cpy r1, r2
1084e63c  0200a0e1  cpy r0, r2
1084e640  53c7feeb  bl 0x10800394
1084e644  1f0058e3  cmp r8, #0x1f
1084e648  030000da  ble 0x1084e65c
1084e64c  0020a0e3  mov r2, #0x0
1084e650  0210a0e1  cpy r1, r2
1084e654  0200a0e1  cpy r0, r2
1084e658  4dc7feeb  bl 0x10800394
1084e65c  1f0059e3  cmp r9, #0x1f
1084e660  030000da  ble 0x1084e674
1084e664  0020a0e3  mov r2, #0x0
1084e668  0210a0e1  cpy r1, r2
1084e66c  0200a0e1  cpy r0, r2
1084e670  47c7feeb  bl 0x10800394
1084e674  871da0e1  mov r1, r7, lsl #0x1b
1084e678  882da0e1  mov r2, r8, lsl #0x1b
1084e67c  a138a0e1  mov r3, r1, lsr #0x11
1084e680  860da0e1  mov r0, r6, lsl #0x1b
1084e684  221ba0e1  mov r1, r2, lsr #0x16
1084e688  200683e1  orr r0, r3, r0, lsr #0xc
1084e68c  1f2009e2  and r2, r9, #0x1f
1084e690  010080e1  orr r0, r0, r1
1084e694  021080e1  orr r1, r0, r2
1084e698  0500a0e1  cpy r0, r5
1084e69c  a3fcffeb  bl 0x1084d930
1084e6a0  100094e5  ldr r0, [r4,#0x10]
1084e6a4  7010ffe6  uxth r1, r0
1084e6a8  0500a0e1  cpy r0, r5
1084e6ac  714e00eb  bl 0x10862078
1084e6b0  140094e5  ldr r0, [r4,#0x14]
1084e6b4  0cd08de2  add sp, sp, #0xc
1084e6b8  7010ffe6  uxth r1, r0
1084e6bc  0500a0e1  cpy r0, r5
1084e6c0  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e6c4  6b4e00ea  b 0x10862078
1084e6c8  00808de5  str r8, [sp,#0x0]
1084e6cc  08108de5  str r1, [sp,#0x8]
1084e6d0  04808de5  str r8, [sp,#0x4]
1084e6d4  0e0094e8  ldmia r4,{r1,r2,r3}
1084e6d8  0200a0e3  mov r0, #0x2
1084e6dc  affcffeb  bl 0x1084d9a0
1084e6e0  7010ffe6  uxth r1, r0
1084e6e4  0500a0e1  cpy r0, r5
1084e6e8  624e00eb  bl 0x10862078
1084e6ec  0c0094e5  ldr r0, [r4,#0xc]
1084e6f0  7010ffe6  uxth r1, r0
1084e6f4  0500a0e1  cpy r0, r5
1084e6f8  5e4e00eb  bl 0x10862078
1084e6fc  100094e5  ldr r0, [r4,#0x10]
1084e700  7010ffe6  uxth r1, r0
1084e704  0500a0e1  cpy r0, r5
1084e708  5a4e00eb  bl 0x10862078
1084e70c  140094e5  ldr r0, [r4,#0x14]
1084e710  0cd08de2  add sp, sp, #0xc
1084e714  7010ffe6  uxth r1, r0
1084e718  0500a0e1  cpy r0, r5
1084e71c  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
1084e720  544e00ea  b 0x10862078
1084e724  0070a0e3  mov r7, #0x0
1084e728  000095e5  ldr r0, [r5,#0x0]
1084e72c  071194e7  ldr r1, [r4,r7,lsl #0x2]
1084e730  090050e1  cmp r0, r9
1084e734  ff8001e2  and r8, r1, #0xff
1084e738  0300003a  bcc 0x1084e74c
1084e73c  0020a0e3  mov r2, #0x0
1084e740  0210a0e1  cpy r1, r2
1084e744  0200a0e1  cpy r0, r2
1084e748  15c7feeb  bl 0x108003a4
1084e74c  000095e5  ldr r0, [r5,#0x0]
1084e750  017087e2  add r7, r7, #0x1
1084e754  070057e3  cmp r7, #0x7
1084e758  012080e2  add r2, r0, #0x1
1084e75c  002085e5  str r2, [r5,#0x0]
1084e760  0680c0e7  strb r8, [r0,r6]
1084e764  efffffba  blt 0x1084e728
1084e768  0cd08de2  add sp, sp, #0xc
1084e76c  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
1084e770  0070a0e3  mov r7, #0x0
1084e774  000095e5  ldr r0, [r5,#0x0]
1084e778  071194e7  ldr r1, [r4,r7,lsl #0x2]
1084e77c  090050e1  cmp r0, r9
1084e780  ff8001e2  and r8, r1, #0xff
1084e784  0300003a  bcc 0x1084e798
1084e788  0020a0e3  mov r2, #0x0
1084e78c  0210a0e1  cpy r1, r2
1084e790  0200a0e1  cpy r0, r2
1084e794  02c7feeb  bl 0x108003a4
1084e798  000095e5  ldr r0, [r5,#0x0]
1084e79c  017087e2  add r7, r7, #0x1
1084e7a0  080057e3  cmp r7, #0x8
1084e7a4  012080e2  add r2, r0, #0x1
1084e7a8  002085e5  str r2, [r5,#0x0]
1084e7ac  0680c0e7  strb r8, [r0,r6]
1084e7b0  efffffba  blt 0x1084e774
1084e7b4  0cd08de2  add sp, sp, #0xc
1084e7b8  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
1084e7bc  000095e5  ldr r0, [r5,#0x0]
1084e7c0  071194e7  ldr r1, [r4,r7,lsl #0x2]
1084e7c4  090050e1  cmp r0, r9
1084e7c8  ff8001e2  and r8, r1, #0xff
1084e7cc  0300003a  bcc 0x1084e7e0
1084e7d0  0020a0e3  mov r2, #0x0
1084e7d4  0210a0e1  cpy r1, r2
1084e7d8  0200a0e1  cpy r0, r2
1084e7dc  f0c6feeb  bl 0x108003a4
1084e7e0  000095e5  ldr r0, [r5,#0x0]
1084e7e4  017087e2  add r7, r7, #0x1
1084e7e8  090057e3  cmp r7, #0x9
1084e7ec  012080e2  add r2, r0, #0x1
1084e7f0  002085e5  str r2, [r5,#0x0]
1084e7f4  0680c0e7  strb r8, [r0,r6]
1084e7f8  efffffba  blt 0x1084e7bc
1084e7fc  0cd08de2  add sp, sp, #0xc
1084e800  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
