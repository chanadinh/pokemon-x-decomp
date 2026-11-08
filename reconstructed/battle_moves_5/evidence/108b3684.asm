108b3684  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b3688  0160a0e1  cpy r6, r1
108b368c  0240a0e1  cpy r4, r2
108b3690  0150a0e3  mov r5, #0x1
108b3694  cd70a0e3  mov r7, #0xcd
108b3698  0300a0e3  mov r0, #0x3
108b369c  9ee0feeb  bl 0x1086b91c
108b36a0  040050e1  cmp r0, r4
108b36a4  1600001a  bne 0x108b3704
108b36a8  0600a0e1  cpy r0, r6
108b36ac  8860feeb  bl 0x1084b8d4
108b36b0  010050e3  cmp r0, #0x1
108b36b4  1200000a  beq 0x108b3704
108b36b8  0420a0e1  cpy r2, r4
108b36bc  1c10a0e3  mov r1, #0x1c
108b36c0  0600a0e1  cpy r0, r6
108b36c4  2cdafeeb  bl 0x10869f7c
108b36c8  0040a0e1  cpy r4, r0
108b36cc  0a00a0e3  mov r0, #0xa
108b36d0  0550c4e5  strb r5, [r4,#0x5]
108b36d4  0400c4e5  strb r0, [r4,#0x4]
108b36d8  0500a0e3  mov r0, #0x5
108b36dc  d661feeb  bl 0x1084be3c
108b36e0  080084e5  str r0, [r4,#0x8]
108b36e4  0720a0e1  cpy r2, r7
108b36e8  0110a0e3  mov r1, #0x1
108b36ec  0c0084e2  add r0, r4, #0xc
108b36f0  f766feeb  bl 0x1084d2d4
108b36f4  0410a0e1  cpy r1, r4
108b36f8  0600a0e1  cpy r0, r6
108b36fc  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b3700  10d1feea  b 0x10867b48
108b3704  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
