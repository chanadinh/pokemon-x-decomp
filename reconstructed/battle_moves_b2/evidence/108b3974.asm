108b3974  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b3978  0160a0e1  cpy r6, r1
108b397c  0250a0e1  cpy r5, r2
108b3980  0300a0e3  mov r0, #0x3
108b3984  e4dffeeb  bl 0x1086b91c
108b3988  050050e1  cmp r0, r5
108b398c  1400001a  bne 0x108b39e4
108b3990  0600a0e3  mov r0, #0x6
108b3994  e0dffeeb  bl 0x1086b91c
108b3998  ff4000e2  and r4, r0, #0xff
108b399c  0410a0e1  cpy r1, r4
108b39a0  0600a0e1  cpy r0, r6
108b39a4  bd82feeb  bl 0x108544a0
108b39a8  0b10a0e3  mov r1, #0xb
108b39ac  ebeffeeb  bl 0x1086f960
108b39b0  000050e3  cmp r0, #0x0
108b39b4  0a00001a  bne 0x108b39e4
108b39b8  0520a0e1  cpy r2, r5
108b39bc  1410a0e3  mov r1, #0x14
108b39c0  0600a0e1  cpy r0, r6
108b39c4  6cd9feeb  bl 0x10869f7c
108b39c8  0010a0e1  cpy r1, r0
108b39cc  0b00a0e3  mov r0, #0xb
108b39d0  0540c1e5  strb r4, [r1,#0x5]
108b39d4  0400c1e5  strb r0, [r1,#0x4]
108b39d8  0600a0e1  cpy r0, r6
108b39dc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b39e0  58d0feea  b 0x10867b48
108b39e4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
