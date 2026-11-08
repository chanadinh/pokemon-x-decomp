108ab254  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ab258  0160a0e1  cpy r6, r1
108ab25c  0250a0e1  cpy r5, r2
108ab260  0400a0e3  mov r0, #0x4
108ab264  ac01ffeb  bl 0x1086b91c
108ab268  050050e1  cmp r0, r5
108ab26c  1400001a  bne 0x108ab2c4
108ab270  0520a0e1  cpy r2, r5
108ab274  2110a0e3  mov r1, #0x21
108ab278  0600a0e1  cpy r0, r6
108ab27c  3efbfeeb  bl 0x10869f7c
108ab280  0040a0e1  cpy r4, r0
108ab284  0010a0e3  mov r1, #0x0
108ab288  b410c4e1  strh r1, [r4,#0x4]
108ab28c  0100a0e3  mov r0, #0x1
108ab290  0650c4e5  strb r5, [r4,#0x6]
108ab294  0a00c4e5  strb r0, [r4,#0xa]
108ab298  28209fe5  ldr r2, [0x108ab2c8]
108ab29c  0210a0e3  mov r1, #0x2
108ab2a0  0c0084e2  add r0, r4, #0xc
108ab2a4  0a88feeb  bl 0x1084d2d4
108ab2a8  0510a0e1  cpy r1, r5
108ab2ac  0c0084e2  add r0, r4, #0xc
108ab2b0  f987feeb  bl 0x1084d29c
108ab2b4  0410a0e1  cpy r1, r4
108ab2b8  0600a0e1  cpy r0, r6
108ab2bc  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ab2c0  20f2feea  b 0x10867b48
108ab2c4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
