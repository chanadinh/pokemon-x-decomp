108ba570  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ba574  0160a0e1  cpy r6, r1
108ba578  0240a0e1  cpy r4, r2
108ba57c  0400a0e3  mov r0, #0x4
108ba580  e5c4feeb  bl 0x1086b91c
108ba584  040050e1  cmp r0, r4
108ba588  2600001a  bne 0x108ba628
108ba58c  1200a0e3  mov r0, #0x12
108ba590  e1c4feeb  bl 0x1086b91c
108ba594  7000ffe6  uxth r0, r0
108ba598  0010a0e3  mov r1, #0x0
108ba59c  be17fdeb  bl 0x1080049c
108ba5a0  000050e3  cmp r0, #0x0
108ba5a4  1f00000a  beq 0x108ba628
108ba5a8  0300a0e3  mov r0, #0x3
108ba5ac  dac4feeb  bl 0x1086b91c
108ba5b0  ff5000e2  and r5, r0, #0xff
108ba5b4  0510a0e1  cpy r1, r5
108ba5b8  0600a0e1  cpy r0, r6
108ba5bc  b767feeb  bl 0x108544a0
108ba5c0  0070a0e1  cpy r7, r0
108ba5c4  0420a0e1  cpy r2, r4
108ba5c8  0610a0e3  mov r1, #0x6
108ba5cc  0600a0e1  cpy r0, r6
108ba5d0  69befeeb  bl 0x10869f7c
108ba5d4  0040a0e1  cpy r4, r0
108ba5d8  0650c0e5  strb r5, [r0,#0x6]
108ba5dc  0810a0e3  mov r1, #0x8
108ba5e0  0700a0e1  cpy r0, r7
108ba5e4  424bfeeb  bl 0x1084d2f4
108ba5e8  0010a0e1  cpy r1, r0
108ba5ec  0700d4e5  ldrb r0, [r4,#0x7]
108ba5f0  b410c4e1  strh r1, [r4,#0x4]
108ba5f4  132da0e3  mov r2, #0x4c0
108ba5f8  0100c0e3  bic r0, r0, #0x1
108ba5fc  0700c4e5  strb r0, [r4,#0x7]
108ba600  0210a0e3  mov r1, #0x2
108ba604  0c0084e2  add r0, r4, #0xc
108ba608  314bfeeb  bl 0x1084d2d4
108ba60c  0510a0e1  cpy r1, r5
108ba610  0c0084e2  add r0, r4, #0xc
108ba614  204bfeeb  bl 0x1084d29c
108ba618  0410a0e1  cpy r1, r4
108ba61c  0600a0e1  cpy r0, r6
108ba620  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ba624  47b5feea  b 0x10867b48
108ba628  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
