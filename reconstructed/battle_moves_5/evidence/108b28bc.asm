108b28bc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b28c0  0160a0e1  cpy r6, r1
108b28c4  0240a0e1  cpy r4, r2
108b28c8  0250a0e3  mov r5, #0x2
108b28cc  cf70a0e3  mov r7, #0xcf
108b28d0  0300a0e3  mov r0, #0x3
108b28d4  10e4feeb  bl 0x1086b91c
108b28d8  040050e1  cmp r0, r4
108b28dc  1600001a  bne 0x108b293c
108b28e0  0600a0e1  cpy r0, r6
108b28e4  fa63feeb  bl 0x1084b8d4
108b28e8  020050e3  cmp r0, #0x2
108b28ec  1200000a  beq 0x108b293c
108b28f0  0420a0e1  cpy r2, r4
108b28f4  1c10a0e3  mov r1, #0x1c
108b28f8  0600a0e1  cpy r0, r6
108b28fc  9eddfeeb  bl 0x10869f7c
108b2900  0040a0e1  cpy r4, r0
108b2904  0a00a0e3  mov r0, #0xa
108b2908  0550c4e5  strb r5, [r4,#0x5]
108b290c  0400c4e5  strb r0, [r4,#0x4]
108b2910  0500a0e3  mov r0, #0x5
108b2914  4865feeb  bl 0x1084be3c
108b2918  080084e5  str r0, [r4,#0x8]
108b291c  0720a0e1  cpy r2, r7
108b2920  0110a0e3  mov r1, #0x1
108b2924  0c0084e2  add r0, r4, #0xc
108b2928  696afeeb  bl 0x1084d2d4
108b292c  0410a0e1  cpy r1, r4
108b2930  0600a0e1  cpy r0, r6
108b2934  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b2938  82d4feea  b 0x10867b48
108b293c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
