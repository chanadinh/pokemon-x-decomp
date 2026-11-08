108b8174  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8178  0160a0e1  cpy r6, r1
108b817c  0240a0e1  cpy r4, r2
108b8180  0300a0e3  mov r0, #0x3
108b8184  e4cdfeeb  bl 0x1086b91c
108b8188  040050e1  cmp r0, r4
108b818c  1900001a  bne 0x108b81f8
108b8190  0420a0e1  cpy r2, r4
108b8194  1710a0e3  mov r1, #0x17
108b8198  0600a0e1  cpy r0, r6
108b819c  76c7feeb  bl 0x10869f7c
108b81a0  0010a0e1  cpy r1, r0
108b81a4  0400a0e3  mov r0, #0x4
108b81a8  0540c1e5  strb r4, [r1,#0x5]
108b81ac  0400c1e5  strb r0, [r1,#0x4]
108b81b0  0600a0e1  cpy r0, r6
108b81b4  63befeeb  bl 0x10867b48
108b81b8  0420a0e1  cpy r2, r4
108b81bc  0310a0e3  mov r1, #0x3
108b81c0  0600a0e1  cpy r0, r6
108b81c4  6cc7feeb  bl 0x10869f7c
108b81c8  0050a0e1  cpy r5, r0
108b81cc  28209fe5  ldr r2, [0x108b81fc]
108b81d0  0210a0e3  mov r1, #0x2
108b81d4  040080e2  add r0, r0, #0x4
108b81d8  3d54feeb  bl 0x1084d2d4
108b81dc  0410a0e1  cpy r1, r4
108b81e0  040085e2  add r0, r5, #0x4
108b81e4  2c54feeb  bl 0x1084d29c
108b81e8  0510a0e1  cpy r1, r5
108b81ec  0600a0e1  cpy r0, r6
108b81f0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b81f4  53befeea  b 0x10867b48
108b81f8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
