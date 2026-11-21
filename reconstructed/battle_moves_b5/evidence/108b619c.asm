108b619c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b61a0  0160a0e1  cpy r6, r1
108b61a4  0240a0e1  cpy r4, r2
108b61a8  0300a0e3  mov r0, #0x3
108b61ac  dad5feeb  bl 0x1086b91c
108b61b0  040050e1  cmp r0, r4
108b61b4  1800001a  bne 0x108b621c
108b61b8  0600a0e3  mov r0, #0x6
108b61bc  d6d5feeb  bl 0x1086b91c
108b61c0  0050a0e1  cpy r5, r0
108b61c4  0420a0e1  cpy r2, r4
108b61c8  2010a0e3  mov r1, #0x20
108b61cc  0600a0e1  cpy r0, r6
108b61d0  69cffeeb  bl 0x10869f7c
108b61d4  0040a0e1  cpy r4, r0
108b61d8  0f00a0e3  mov r0, #0xf
108b61dc  0650c4e5  strb r5, [r4,#0x6]
108b61e0  b400c4e1  strh r0, [r4,#0x4]
108b61e4  34209fe5  ldr r2, [0x108b6220]
108b61e8  0210a0e3  mov r1, #0x2
108b61ec  0c0084e2  add r0, r4, #0xc
108b61f0  375cfeeb  bl 0x1084d2d4
108b61f4  0610d4e5  ldrb r1, [r4,#0x6]
108b61f8  0c0084e2  add r0, r4, #0xc
108b61fc  265cfeeb  bl 0x1084d29c
108b6200  b410d4e1  ldrh r1, [r4,#0x4]
108b6204  0c0084e2  add r0, r4, #0xc
108b6208  235cfeeb  bl 0x1084d29c
108b620c  0410a0e1  cpy r1, r4
108b6210  0600a0e1  cpy r0, r6
108b6214  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b6218  4ac6feea  b 0x10867b48
108b621c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
