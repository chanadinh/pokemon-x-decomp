108af13c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108af140  0160a0e1  cpy r6, r1
108af144  0240a0e1  cpy r4, r2
108af148  0300a0e3  mov r0, #0x3
108af14c  f2f1feeb  bl 0x1086b91c
108af150  040050e1  cmp r0, r4
108af154  2000001a  bne 0x108af1dc
108af158  0600a0e3  mov r0, #0x6
108af15c  eef1feeb  bl 0x1086b91c
108af160  ff5000e2  and r5, r0, #0xff
108af164  0510a0e1  cpy r1, r5
108af168  0600a0e1  cpy r0, r6
108af16c  cb94feeb  bl 0x108544a0
108af170  0110a0e3  mov r1, #0x1
108af174  a775feeb  bl 0x1084c818
108af178  000050e3  cmp r0, #0x0
108af17c  1600001a  bne 0x108af1dc
108af180  0420a0e1  cpy r2, r4
108af184  0b10a0e3  mov r1, #0xb
108af188  0600a0e1  cpy r0, r6
108af18c  7aebfeeb  bl 0x10869f7c
108af190  0040a0e1  cpy r4, r0
108af194  2400a0e3  mov r0, #0x24
108af198  0f50c4e5  strb r5, [r4,#0xf]
108af19c  b400c4e1  strh r0, [r4,#0x4]
108af1a0  0c10a0e3  mov r1, #0xc
108af1a4  0100a0e3  mov r0, #0x1
108af1a8  3f73feeb  bl 0x1084beac
108af1ac  080084e5  str r0, [r4,#0x8]
108af1b0  28209fe5  ldr r2, [0x108af1e0]
108af1b4  0210a0e3  mov r1, #0x2
108af1b8  140084e2  add r0, r4, #0x14
108af1bc  4478feeb  bl 0x1084d2d4
108af1c0  0510a0e1  cpy r1, r5
108af1c4  140084e2  add r0, r4, #0x14
108af1c8  3378feeb  bl 0x1084d29c
108af1cc  0410a0e1  cpy r1, r4
108af1d0  0600a0e1  cpy r0, r6
108af1d4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108af1d8  5ae2feea  b 0x10867b48
108af1dc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
