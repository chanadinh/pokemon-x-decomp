108bfb58  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bfb5c  0160a0e1  cpy r6, r1
108bfb60  0250a0e1  cpy r5, r2
108bfb64  3a00a0e3  mov r0, #0x3a
108bfb68  6baffeeb  bl 0x1086b91c
108bfb6c  020050e3  cmp r0, #0x2
108bfb70  1b00001a  bne 0x108bfbe4
108bfb74  0200a0e3  mov r0, #0x2
108bfb78  67affeeb  bl 0x1086b91c
108bfb7c  050050e1  cmp r0, r5
108bfb80  1700001a  bne 0x108bfbe4
108bfb84  0510a0e1  cpy r1, r5
108bfb88  0600a0e1  cpy r0, r6
108bfb8c  4352feeb  bl 0x108544a0
108bfb90  0070a0e1  cpy r7, r0
108bfb94  0520a0e1  cpy r2, r5
108bfb98  0410a0e3  mov r1, #0x4
108bfb9c  0600a0e1  cpy r0, r6
108bfba0  f5a8feeb  bl 0x10869f7c
108bfba4  0040a0e1  cpy r4, r0
108bfba8  000090e5  ldr r0, [r0,#0x0]
108bfbac  0650c4e5  strb r5, [r4,#0x6]
108bfbb0  1010a0e3  mov r1, #0x10
108bfbb4  020580e3  orr r0, r0, #0x800000
108bfbb8  000084e5  str r0, [r4,#0x0]
108bfbbc  0700a0e1  cpy r0, r7
108bfbc0  cb35feeb  bl 0x1084d2f4
108bfbc4  b400c4e1  strh r0, [r4,#0x4]
108bfbc8  0410a0e1  cpy r1, r4
108bfbcc  0600a0e1  cpy r0, r6
108bfbd0  dc9ffeeb  bl 0x10867b48
108bfbd4  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bfbd8  0110a0e3  mov r1, #0x1
108bfbdc  4200a0e3  mov r0, #0x42
108bfbe0  3b97feea  b 0x108658d4
108bfbe4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
