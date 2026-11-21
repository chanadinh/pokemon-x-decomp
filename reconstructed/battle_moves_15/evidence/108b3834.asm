108b3834  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b3838  0160a0e1  cpy r6, r1
108b383c  0250a0e1  cpy r5, r2
108b3840  0200a0e3  mov r0, #0x2
108b3844  34e0feeb  bl 0x1086b91c
108b3848  050050e1  cmp r0, r5
108b384c  1300001a  bne 0x108b38a0
108b3850  0520a0e1  cpy r2, r5
108b3854  0b10a0e3  mov r1, #0xb
108b3858  0600a0e1  cpy r0, r6
108b385c  c6d9feeb  bl 0x10869f7c
108b3860  0040a0e1  cpy r4, r0
108b3864  0100a0e3  mov r0, #0x1
108b3868  0f50c4e5  strb r5, [r4,#0xf]
108b386c  1810a0e3  mov r1, #0x18
108b3870  0d00c4e5  strb r0, [r4,#0xd]
108b3874  b410c4e1  strh r1, [r4,#0x4]
108b3878  0210a0e3  mov r1, #0x2
108b387c  8a61feeb  bl 0x1084beac
108b3880  080084e5  str r0, [r4,#0x8]
108b3884  0110a0e3  mov r1, #0x1
108b3888  080084e2  add r0, r4, #0x8
108b388c  050dffeb  bl 0x10876ca8
108b3890  0410a0e1  cpy r1, r4
108b3894  0600a0e1  cpy r0, r6
108b3898  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b389c  a9d0feea  b 0x10867b48
108b38a0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
