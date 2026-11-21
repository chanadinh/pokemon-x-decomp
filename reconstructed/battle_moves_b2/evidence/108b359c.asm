108b359c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b35a0  0160a0e1  cpy r6, r1
108b35a4  0250a0e1  cpy r5, r2
108b35a8  0300a0e3  mov r0, #0x3
108b35ac  dae0feeb  bl 0x1086b91c
108b35b0  050050e1  cmp r0, r5
108b35b4  1500001a  bne 0x108b3610
108b35b8  0520a0e1  cpy r2, r5
108b35bc  0b10a0e3  mov r1, #0xb
108b35c0  0600a0e1  cpy r0, r6
108b35c4  6cdafeeb  bl 0x10869f7c
108b35c8  0040a0e1  cpy r4, r0
108b35cc  1e00a0e3  mov r0, #0x1e
108b35d0  0f50c4e5  strb r5, [r4,#0xf]
108b35d4  b400c4e1  strh r0, [r4,#0x4]
108b35d8  0500a0e3  mov r0, #0x5
108b35dc  1662feeb  bl 0x1084be3c
108b35e0  080084e5  str r0, [r4,#0x8]
108b35e4  28209fe5  ldr r2, [0x108b3614]
108b35e8  0210a0e3  mov r1, #0x2
108b35ec  140084e2  add r0, r4, #0x14
108b35f0  3767feeb  bl 0x1084d2d4
108b35f4  0510a0e1  cpy r1, r5
108b35f8  140084e2  add r0, r4, #0x14
108b35fc  2667feeb  bl 0x1084d29c
108b3600  0410a0e1  cpy r1, r4
108b3604  0600a0e1  cpy r0, r6
108b3608  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b360c  4dd1feea  b 0x10867b48
108b3610  7080bde8  ldmia sp!,{r4,r5,r6,pc}
