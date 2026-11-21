108b69f4  f04f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,lr}
108b69f8  0cd04de2  sub sp, sp, #0xc
108b69fc  0280a0e1  cpy r8, r2
108b6a00  0190a0e1  cpy r9, r1
108b6a04  0300a0e3  mov r0, #0x3
108b6a08  c3d3feeb  bl 0x1086b91c
108b6a0c  080050e1  cmp r0, r8
108b6a10  3300001a  bne 0x108b6ae4
108b6a14  0400a0e3  mov r0, #0x4
108b6a18  bfd3feeb  bl 0x1086b91c
108b6a1c  ff7000e2  and r7, r0, #0xff
108b6a20  0710a0e1  cpy r1, r7
108b6a24  0900a0e1  cpy r0, r9
108b6a28  d852feeb  bl 0x1084b590
108b6a2c  0a0050e3  cmp r0, #0xa
108b6a30  2b00000a  beq 0x108b6ae4
108b6a34  091c80e3  orr r1, r0, #0x900
108b6a38  0d20a0e1  cpy r2, sp
108b6a3c  0900a0e1  cpy r0, r9
108b6a40  7654feeb  bl 0x1084bc20
108b6a44  006050e2  subs r6, r0, #0x0
108b6a48  9cb09f85  ldrhi r11, [0x108b6aec]
108b6a4c  0040a0e3  mov r4, #0x0
108b6a50  0da0a081  cpyhi r10, sp
108b6a54  2200009a  bls 0x108b6ae4
108b6a58  0410dae7  ldrb r1, [r10,r4]
108b6a5c  070051e1  cmp r1, r7
108b6a60  1b00000a  beq 0x108b6ad4
108b6a64  0900a0e1  cpy r0, r9
108b6a68  8c76feeb  bl 0x108544a0
108b6a6c  08008de5  str r0, [sp,#0x8]
108b6a70  0820a0e1  cpy r2, r8
108b6a74  0610a0e3  mov r1, #0x6
108b6a78  0900a0e1  cpy r0, r9
108b6a7c  3ecdfeeb  bl 0x10869f7c
108b6a80  0050a0e1  cpy r5, r0
108b6a84  0400dae7  ldrb r0, [r10,r4]
108b6a88  1010a0e3  mov r1, #0x10
108b6a8c  0600c5e5  strb r0, [r5,#0x6]
108b6a90  08009de5  ldr r0, [sp,#0x8]
108b6a94  165afeeb  bl 0x1084d2f4
108b6a98  0010a0e1  cpy r1, r0
108b6a9c  0700d5e5  ldrb r0, [r5,#0x7]
108b6aa0  b410c5e1  strh r1, [r5,#0x4]
108b6aa4  0b20a0e1  cpy r2, r11
108b6aa8  010080e3  orr r0, r0, #0x1
108b6aac  0700c5e5  strb r0, [r5,#0x7]
108b6ab0  0210a0e3  mov r1, #0x2
108b6ab4  0c0085e2  add r0, r5, #0xc
108b6ab8  055afeeb  bl 0x1084d2d4
108b6abc  0610d5e5  ldrb r1, [r5,#0x6]
108b6ac0  0c0085e2  add r0, r5, #0xc
108b6ac4  f459feeb  bl 0x1084d29c
108b6ac8  0510a0e1  cpy r1, r5
108b6acc  0900a0e1  cpy r0, r9
108b6ad0  1cc4feeb  bl 0x10867b48
108b6ad4  010084e2  add r0, r4, #0x1
108b6ad8  ff4000e2  and r4, r0, #0xff
108b6adc  060054e1  cmp r4, r6
108b6ae0  dcffff3a  bcc 0x108b6a58
108b6ae4  0cd08de2  add sp, sp, #0xc
108b6ae8  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
