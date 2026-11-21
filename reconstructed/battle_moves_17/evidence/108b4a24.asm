108b4a24  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b4a28  0160a0e1  cpy r6, r1
108b4a2c  0240a0e1  cpy r4, r2
108b4a30  0300a0e3  mov r0, #0x3
108b4a34  b8dbfeeb  bl 0x1086b91c
108b4a38  040050e1  cmp r0, r4
108b4a3c  2800001a  bne 0x108b4ae4
108b4a40  4700a0e3  mov r0, #0x47
108b4a44  b4dbfeeb  bl 0x1086b91c
108b4a48  000050e3  cmp r0, #0x0
108b4a4c  2400001a  bne 0x108b4ae4
108b4a50  0400a0e3  mov r0, #0x4
108b4a54  b0dbfeeb  bl 0x1086b91c
108b4a58  ff5000e2  and r5, r0, #0xff
108b4a5c  0510a0e1  cpy r1, r5
108b4a60  0600a0e1  cpy r0, r6
108b4a64  8d7efeeb  bl 0x108544a0
108b4a68  b201d0e1  ldrh r0, [r0,#0x12]
108b4a6c  0070a0e1  cpy r7, r0
108b4a70  4b2ffdeb  bl 0x108007a4
108b4a74  000050e3  cmp r0, #0x0
108b4a78  0300001a  bne 0x108b4a8c
108b4a7c  0700a0e1  cpy r0, r7
108b4a80  9d34fdeb  bl 0x10801cfc
108b4a84  000050e3  cmp r0, #0x0
108b4a88  1500000a  beq 0x108b4ae4
108b4a8c  0420a0e1  cpy r2, r4
108b4a90  2110a0e3  mov r1, #0x21
108b4a94  0600a0e1  cpy r0, r6
108b4a98  37d5feeb  bl 0x10869f7c
108b4a9c  0040a0e1  cpy r4, r0
108b4aa0  0000a0e3  mov r0, #0x0
108b4aa4  0650c4e5  strb r5, [r4,#0x6]
108b4aa8  b400c4e1  strh r0, [r4,#0x4]
108b4aac  34209fe5  ldr r2, [0x108b4ae8]
108b4ab0  0210a0e3  mov r1, #0x2
108b4ab4  0c0084e2  add r0, r4, #0xc
108b4ab8  0562feeb  bl 0x1084d2d4
108b4abc  0510a0e1  cpy r1, r5
108b4ac0  0c0084e2  add r0, r4, #0xc
108b4ac4  f461feeb  bl 0x1084d29c
108b4ac8  0710a0e1  cpy r1, r7
108b4acc  0c0084e2  add r0, r4, #0xc
108b4ad0  f161feeb  bl 0x1084d29c
108b4ad4  0410a0e1  cpy r1, r4
108b4ad8  0600a0e1  cpy r0, r6
108b4adc  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b4ae0  18ccfeea  b 0x10867b48
108b4ae4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
