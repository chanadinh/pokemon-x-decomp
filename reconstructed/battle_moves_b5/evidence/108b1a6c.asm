108b1a6c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b1a70  0160a0e1  cpy r6, r1
108b1a74  0250a0e1  cpy r5, r2
108b1a78  0300a0e3  mov r0, #0x3
108b1a7c  a6e7feeb  bl 0x1086b91c
108b1a80  050050e1  cmp r0, r5
108b1a84  1400001a  bne 0x108b1adc
108b1a88  0600a0e3  mov r0, #0x6
108b1a8c  a2e7feeb  bl 0x1086b91c
108b1a90  ff4000e2  and r4, r0, #0xff
108b1a94  0410a0e1  cpy r1, r4
108b1a98  0600a0e1  cpy r0, r6
108b1a9c  7f8afeeb  bl 0x108544a0
108b1aa0  0710a0e3  mov r1, #0x7
108b1aa4  adf7feeb  bl 0x1086f960
108b1aa8  000050e3  cmp r0, #0x0
108b1aac  0a00001a  bne 0x108b1adc
108b1ab0  0520a0e1  cpy r2, r5
108b1ab4  1410a0e3  mov r1, #0x14
108b1ab8  0600a0e1  cpy r0, r6
108b1abc  2ee1feeb  bl 0x10869f7c
108b1ac0  0010a0e1  cpy r1, r0
108b1ac4  0700a0e3  mov r0, #0x7
108b1ac8  0540c1e5  strb r4, [r1,#0x5]
108b1acc  0400c1e5  strb r0, [r1,#0x4]
108b1ad0  0600a0e1  cpy r0, r6
108b1ad4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b1ad8  1ad8feea  b 0x10867b48
108b1adc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
