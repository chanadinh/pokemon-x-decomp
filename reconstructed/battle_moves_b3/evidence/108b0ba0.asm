108b0ba0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b0ba4  0250a0e1  cpy r5, r2
108b0ba8  0180a0e1  cpy r8, r1
108b0bac  0300a0e3  mov r0, #0x3
108b0bb0  59ebfeeb  bl 0x1086b91c
108b0bb4  050050e1  cmp r0, r5
108b0bb8  2400001a  bne 0x108b0c50
108b0bbc  0600a0e3  mov r0, #0x6
108b0bc0  55ebfeeb  bl 0x1086b91c
108b0bc4  ff7000e2  and r7, r0, #0xff
108b0bc8  0710a0e1  cpy r1, r7
108b0bcc  0800a0e1  cpy r0, r8
108b0bd0  328efeeb  bl 0x108544a0
108b0bd4  1010a0e3  mov r1, #0x10
108b0bd8  100effeb  bl 0x10874420
108b0bdc  ff6000e2  and r6, r0, #0xff
108b0be0  0600a0e1  cpy r0, r6
108b0be4  3ecfffeb  bl 0x108a48e4
108b0be8  000050e3  cmp r0, #0x0
108b0bec  1700001a  bne 0x108b0c50
108b0bf0  0520a0e1  cpy r2, r5
108b0bf4  2010a0e3  mov r1, #0x20
108b0bf8  0800a0e1  cpy r0, r8
108b0bfc  dee4feeb  bl 0x10869f7c
108b0c00  0650c0e5  strb r5, [r0,#0x6]
108b0c04  0040a0e1  cpy r4, r0
108b0c08  b460c0e1  strh r6, [r0,#0x4]
108b0c0c  40209fe5  ldr r2, [0x108b0c54]
108b0c10  0210a0e3  mov r1, #0x2
108b0c14  0c0080e2  add r0, r0, #0xc
108b0c18  ad71feeb  bl 0x1084d2d4
108b0c1c  0510a0e1  cpy r1, r5
108b0c20  0c0084e2  add r0, r4, #0xc
108b0c24  9c71feeb  bl 0x1084d29c
108b0c28  0710a0e1  cpy r1, r7
108b0c2c  0c0084e2  add r0, r4, #0xc
108b0c30  9971feeb  bl 0x1084d29c
108b0c34  b410d4e1  ldrh r1, [r4,#0x4]
108b0c38  0c0084e2  add r0, r4, #0xc
108b0c3c  9671feeb  bl 0x1084d29c
108b0c40  0410a0e1  cpy r1, r4
108b0c44  0800a0e1  cpy r0, r8
108b0c48  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b0c4c  bddbfeea  b 0x10867b48
108b0c50  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
