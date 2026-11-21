108af690  70402de9  stmdb sp!,{r4,r5,r6,lr}
108af694  0160a0e1  cpy r6, r1
108af698  0240a0e1  cpy r4, r2
108af69c  0300a0e3  mov r0, #0x3
108af6a0  9df0feeb  bl 0x1086b91c
108af6a4  040050e1  cmp r0, r4
108af6a8  1700001a  bne 0x108af70c
108af6ac  0600a0e3  mov r0, #0x6
108af6b0  99f0feeb  bl 0x1086b91c
108af6b4  ff5000e2  and r5, r0, #0xff
108af6b8  0510a0e1  cpy r1, r5
108af6bc  0600a0e1  cpy r0, r6
108af6c0  7693feeb  bl 0x108544a0
108af6c4  0c10a0e3  mov r1, #0xc
108af6c8  ee9cfeeb  bl 0x10856a88
108af6cc  000050e3  cmp r0, #0x0
108af6d0  0d00001a  bne 0x108af70c
108af6d4  0420a0e1  cpy r2, r4
108af6d8  0b10a0e3  mov r1, #0xb
108af6dc  0600a0e1  cpy r0, r6
108af6e0  25eafeeb  bl 0x10869f7c
108af6e4  0040a0e1  cpy r4, r0
108af6e8  0c00a0e3  mov r0, #0xc
108af6ec  b400c4e1  strh r0, [r4,#0x4]
108af6f0  0100a0e3  mov r0, #0x1
108af6f4  080084e5  str r0, [r4,#0x8]
108af6f8  0410a0e1  cpy r1, r4
108af6fc  0600a0e1  cpy r0, r6
108af700  0f50c4e5  strb r5, [r4,#0xf]
108af704  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108af708  0ee1feea  b 0x10867b48
108af70c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
