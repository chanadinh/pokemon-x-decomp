1084bcb0  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1084bcb4  0300a0e3  mov r0, #0x3
1084bcb8  0180a0e1  cpy r8, r1
1084bcbc  0190a0e3  mov r9, #0x1
1084bcc0  028b2ded  vpush {d8}
1084bcc4  28408de2  add r4, sp, #0x28
1084bcc8  34709de5  ldr r7, [sp,#0x34]
1084bccc  600094e8  ldmia r4,{r5,r6}
1084bcd0  0240a0e1  cpy r4, r2
1084bcd4  0c8a9ded  vldr.32 s16, [sp,#0x30]
1084bcd8  0f7f00eb  bl 0x1086b91c
1084bcdc  040050e1  cmp r0, r4
1084bce0  1300001a  bne 0x1084bd34
1084bce4  0420a0e1  cpy r2, r4
1084bce8  1910a0e3  mov r1, #0x19
1084bcec  0800a0e1  cpy r0, r8
1084bcf0  a17800eb  bl 0x10869f7c
1084bcf4  0460c0e5  strb r6, [r0,#0x4]
1084bcf8  0c50c0e5  strb r5, [r0,#0xc]
1084bcfc  0040a0e1  cpy r4, r0
1084bd00  028a80ed  vstr.32 s16, [r0,#0x8]
1084bd04  7720ffe6  uxth r2, r7
1084bd08  0910a0e1  cpy r1, r9
1084bd0c  100080e2  add r0, r0, #0x10
1084bd10  6f0500eb  bl 0x1084d2d4
1084bd14  0510a0e1  cpy r1, r5
1084bd18  100084e2  add r0, r4, #0x10
1084bd1c  5e0500eb  bl 0x1084d29c
1084bd20  028bbdec  vpop {d8}
1084bd24  0410a0e1  cpy r1, r4
1084bd28  0800a0e1  cpy r0, r8
1084bd2c  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1084bd30  846f00ea  b 0x10867b48
1084bd34  028bbdec  vpop {d8}
1084bd38  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
