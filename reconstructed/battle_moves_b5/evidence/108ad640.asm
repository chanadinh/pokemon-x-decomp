108ad640  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ad644  0160a0e1  cpy r6, r1
108ad648  0240a0e1  cpy r4, r2
108ad64c  0300a0e3  mov r0, #0x3
108ad650  b1f8feeb  bl 0x1086b91c
108ad654  040050e1  cmp r0, r4
108ad658  1700001a  bne 0x108ad6bc
108ad65c  0600a0e3  mov r0, #0x6
108ad660  adf8feeb  bl 0x1086b91c
108ad664  ff5000e2  and r5, r0, #0xff
108ad668  0420a0e1  cpy r2, r4
108ad66c  0b10a0e3  mov r1, #0xb
108ad670  0600a0e1  cpy r0, r6
108ad674  40f2feeb  bl 0x10869f7c
108ad678  0040a0e1  cpy r4, r0
108ad67c  1000a0e3  mov r0, #0x10
108ad680  0f50c4e5  strb r5, [r4,#0xf]
108ad684  b400c4e1  strh r0, [r4,#0x4]
108ad688  0100a0e3  mov r0, #0x1
108ad68c  080084e5  str r0, [r4,#0x8]
108ad690  28209fe5  ldr r2, [0x108ad6c0]
108ad694  0210a0e3  mov r1, #0x2
108ad698  140084e2  add r0, r4, #0x14
108ad69c  0c7ffeeb  bl 0x1084d2d4
108ad6a0  0510a0e1  cpy r1, r5
108ad6a4  140084e2  add r0, r4, #0x14
108ad6a8  fb7efeeb  bl 0x1084d29c
108ad6ac  0410a0e1  cpy r1, r4
108ad6b0  0600a0e1  cpy r0, r6
108ad6b4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ad6b8  22e9feea  b 0x10867b48
108ad6bc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
