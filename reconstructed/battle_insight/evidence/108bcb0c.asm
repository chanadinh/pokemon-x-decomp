108bcb0c  ff4f2de9  stmdb sp!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,lr}
108bcb10  14d04de2  sub sp, sp, #0x14
108bcb14  0200a0e3  mov r0, #0x2
108bcb18  0270a0e1  cpy r7, r2
108bcb1c  0380a0e1  cpy r8, r3
108bcb20  7dbbfeeb  bl 0x1086b91c
108bcb24  ff4000e2  and r4, r0, #0xff
108bcb28  070054e1  cmp r4, r7
108bcb2c  0190a0e3  mov r9, #0x1
108bcb30  0710a0e1  cpy r1, r7
108bcb34  5000001a  bne 0x108bcc7c
108bcb38  18009de5  ldr r0, [sp,#0x18]
108bcb3c  0d40feeb  bl 0x1084cb78
108bcb40  011c80e3  orr r1, r0, #0x100
108bcb44  18009de5  ldr r0, [sp,#0x18]
108bcb48  0d20a0e1  cpy r2, sp
108bcb4c  333cfeeb  bl 0x1084bc20
108bcb50  006050e2  subs r6, r0, #0x0
108bcb54  0050a0e3  mov r5, #0x0
108bcb58  0540a0e1  cpy r4, r5
108bcb5c  0da0a081  cpyhi r10, sp
108bcb60  08b08d82  addhi r11, sp, #0x8
108bcb64  4100009a  bls 0x108bcc70
108bcb68  0410dae7  ldrb r1, [r10,r4]
108bcb6c  18009de5  ldr r0, [sp,#0x18]
108bcb70  4a5efeeb  bl 0x108544a0
108bcb74  1010a0e3  mov r1, #0x10
108bcb78  00f020e3  nop
108bcb7c  27defeeb  bl 0x10874420
108bcb80  ff0000e2  and r0, r0, #0xff
108bcb84  00f020e3  nop
108bcb88  319fffeb  bl 0x108a4854
108bcb8c  000050e3  cmp r0, #0x0
108bcb90  00f020e3  nop
108bcb94  0300001a  bne 0x108bcba8
108bcb98  0410dae7  ldrb r1, [r10,r4]
108bcb9c  010085e2  add r0, r5, #0x1
108bcba0  0510cbe7  strb r1, [r11,r5]
108bcba4  ff5000e2  and r5, r0, #0xff
108bcba8  010084e2  add r0, r4, #0x1
108bcbac  ff4000e2  and r4, r0, #0xff
108bcbb0  040056e1  cmp r6, r4
108bcbb4  ebffff8a  bhi 0x108bcb68
108bcbb8  000055e3  cmp r5, #0x0
108bcbbc  2b00000a  beq 0x108bcc70
108bcbc0  010055e3  cmp r5, #0x1
108bcbc4  0500a011  cpyne r0, r5
108bcbc8  0000a003  moveq r0, #0x0
108bcbcc  fa3efe1b  blne 0x1084c7bc
108bcbd0  ff0000e2  and r0, r0, #0xff
108bcbd4  0010dbe7  ldrb r1, [r11,r0]
108bcbd8  18009de5  ldr r0, [sp,#0x18]
108bcbdc  2f5efeeb  bl 0x108544a0
108bcbe0  0040a0e1  cpy r4, r0
108bcbe4  1010a0e3  mov r1, #0x10
108bcbe8  0cdefeeb  bl 0x10874420
108bcbec  ff5000e2  and r5, r0, #0xff
108bcbf0  0400a0e1  cpy r0, r4
108bcbf4  1900d0e5  ldrb r0, [r0,#0x19]
108bcbf8  0060a0e1  cpy r6, r0
108bcbfc  000055e3  cmp r5, #0x0
108bcc00  1f005713  cmpne r7, #0x1f
108bcc04  1a00000a  beq 0x108bcc74
108bcc08  18009de5  ldr r0, [sp,#0x18]
108bcc0c  0720a0e1  cpy r2, r7
108bcc10  2010a0e3  mov r1, #0x20
108bcc14  d8b4feeb  bl 0x10869f7c
108bcc18  0040a0e1  cpy r4, r0
108bcc1c  000090e5  ldr r0, [r0,#0x0]
108bcc20  0670c4e5  strb r7, [r4,#0x6]
108bcc24  b450c4e1  strh r5, [r4,#0x4]
108bcc28  020580e3  orr r0, r0, #0x800000
108bcc2c  0890c4e5  strb r9, [r4,#0x8]
108bcc30  000084e5  str r0, [r4,#0x0]
108bcc34  e8209fe5  ldr r2, [0x108bcd24]
108bcc38  0210a0e3  mov r1, #0x2
108bcc3c  0c0084e2  add r0, r4, #0xc
108bcc40  a341feeb  bl 0x1084d2d4
108bcc44  0610a0e1  cpy r1, r6
108bcc48  0c0084e2  add r0, r4, #0xc
108bcc4c  9241feeb  bl 0x1084d29c
108bcc50  b410d4e1  ldrh r1, [r4,#0x4]
108bcc54  0c0084e2  add r0, r4, #0xc
108bcc58  8f41feeb  bl 0x1084d29c
108bcc5c  18009de5  ldr r0, [sp,#0x18]
108bcc60  24d08de2  add sp, sp, #0x24
108bcc64  0410a0e1  cpy r1, r4
108bcc68  f04fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
108bcc6c  b5abfeea  b 0x10867b48
108bcc70  009088e5  str r9, [r8,#0x0]
108bcc74  24d08de2  add sp, sp, #0x24
108bcc78  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
108bcc7c  0400a0e1  cpy r0, r4
108bcc80  483efeeb  bl 0x1084c5a8
108bcc84  000050e3  cmp r0, #0x0
108bcc88  00f020e3  nop
108bcc8c  f8ffff1a  bne 0x108bcc74
108bcc90  000098e5  ldr r0, [r8,#0x0]
108bcc94  010050e3  cmp r0, #0x1
108bcc98  f5ffff1a  bne 0x108bcc74
108bcc9c  18009de5  ldr r0, [sp,#0x18]
108bcca0  0410a0e1  cpy r1, r4
108bcca4  2bd9feeb  bl 0x10873158
108bcca8  0050a0e1  cpy r5, r0
108bccac  18009de5  ldr r0, [sp,#0x18]
108bccb0  0710a0e1  cpy r1, r7
108bccb4  af3ffeeb  bl 0x1084cb78
108bccb8  0a0055e3  cmp r5, #0xa
108bccbc  0060a0e1  cpy r6, r0
108bccc0  ebffff0a  beq 0x108bcc74
108bccc4  18009de5  ldr r0, [sp,#0x18]
108bccc8  913cfeeb  bl 0x1084bf14
108bcccc  020050e3  cmp r0, #0x2
108bccd0  00f020e3  nop
108bccd4  0500001a  bne 0x108bccf0
108bccd8  0510a0e1  cpy r1, r5
108bccdc  0600a0e1  cpy r0, r6
108bcce0  c4a9feeb  bl 0x108673f8
108bcce4  000050e3  cmp r0, #0x0
108bcce8  00f020e3  nop
108bccec  e0ffff0a  beq 0x108bcc74
108bccf0  18009de5  ldr r0, [sp,#0x18]
108bccf4  0410a0e1  cpy r1, r4
108bccf8  e85dfeeb  bl 0x108544a0
108bccfc  1010a0e3  mov r1, #0x10
108bcd00  00f020e3  nop
108bcd04  c5ddfeeb  bl 0x10874420
108bcd08  ff5000e2  and r5, r0, #0xff
108bcd0c  0500a0e1  cpy r0, r5
108bcd10  cf9effeb  bl 0x108a4854
108bcd14  000050e3  cmp r0, #0x0
108bcd18  0460a001  cpyeq r6, r4
108bcd1c  d4ffff1a  bne 0x108bcc74
108bcd20  b5ffffea  b 0x108bcbfc
