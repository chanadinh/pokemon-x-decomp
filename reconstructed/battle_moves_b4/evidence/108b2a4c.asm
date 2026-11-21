108b2a4c  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b2a50  0250a0e1  cpy r5, r2
108b2a54  0170a0e1  cpy r7, r1
108b2a58  0300a0e3  mov r0, #0x3
108b2a5c  aee3feeb  bl 0x1086b91c
108b2a60  050050e1  cmp r0, r5
108b2a64  2f00001a  bne 0x108b2b28
108b2a68  0600a0e3  mov r0, #0x6
108b2a6c  aae3feeb  bl 0x1086b91c
108b2a70  0040a0e1  cpy r4, r0
108b2a74  0700a0e1  cpy r0, r7
108b2a78  6fabfdeb  bl 0x1081d83c
108b2a7c  0060a0e1  cpy r6, r0
108b2a80  0520a0e1  cpy r2, r5
108b2a84  0d10a0e3  mov r1, #0xd
108b2a88  0700a0e1  cpy r0, r7
108b2a8c  3addfeeb  bl 0x10869f7c
108b2a90  0010a0e1  cpy r1, r0
108b2a94  1800d0e5  ldrb r0, [r0,#0x18]
108b2a98  0180a0e3  mov r8, #0x1
108b2a9c  0c40c1e5  strb r4, [r1,#0xc]
108b2aa0  1680c1e5  strb r8, [r1,#0x16]
108b2aa4  0190e0e3  mvn r9, #0x1
108b2aa8  0480c1e5  strb r8, [r1,#0x4]
108b2aac  160080e3  orr r0, r0, #0x16
108b2ab0  1790c1e5  strb r9, [r1,#0x17]
108b2ab4  1800c1e5  strb r0, [r1,#0x18]
108b2ab8  0700a0e1  cpy r0, r7
108b2abc  086081e5  str r6, [r1,#0x8]
108b2ac0  20d4feeb  bl 0x10867b48
108b2ac4  0520a0e1  cpy r2, r5
108b2ac8  0d10a0e3  mov r1, #0xd
108b2acc  0700a0e1  cpy r0, r7
108b2ad0  29ddfeeb  bl 0x10869f7c
108b2ad4  0010a0e1  cpy r1, r0
108b2ad8  1800d0e5  ldrb r0, [r0,#0x18]
108b2adc  0320a0e3  mov r2, #0x3
108b2ae0  1680c1e5  strb r8, [r1,#0x16]
108b2ae4  0420c1e5  strb r2, [r1,#0x4]
108b2ae8  1790c1e5  strb r9, [r1,#0x17]
108b2aec  0c40c1e5  strb r4, [r1,#0xc]
108b2af0  160080e3  orr r0, r0, #0x16
108b2af4  086081e5  str r6, [r1,#0x8]
108b2af8  1800c1e5  strb r0, [r1,#0x18]
108b2afc  0700a0e1  cpy r0, r7
108b2b00  10d4feeb  bl 0x10867b48
108b2b04  0520a0e1  cpy r2, r5
108b2b08  1210a0e3  mov r1, #0x12
108b2b0c  0700a0e1  cpy r0, r7
108b2b10  19ddfeeb  bl 0x10869f7c
108b2b14  0010a0e1  cpy r1, r0
108b2b18  0450c0e5  strb r5, [r0,#0x4]
108b2b1c  0700a0e1  cpy r0, r7
108b2b20  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b2b24  07d4feea  b 0x10867b48
108b2b28  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
