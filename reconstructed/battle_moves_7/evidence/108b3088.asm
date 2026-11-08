108b3088  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b308c  0060a0e1  cpy r6, r0
108b3090  0150a0e1  cpy r5, r1
108b3094  0240a0e1  cpy r4, r2
108b3098  0300a0e3  mov r0, #0x3
108b309c  1ee2feeb  bl 0x1086b91c
108b30a0  040050e1  cmp r0, r4
108b30a4  1200001a  bne 0x108b30f4
108b30a8  0420a0e1  cpy r2, r4
108b30ac  2f10a0e3  mov r1, #0x2f
108b30b0  0500a0e1  cpy r0, r5
108b30b4  b0dbfeeb  bl 0x10869f7c
108b30b8  0040a0e1  cpy r4, r0
108b30bc  0600a0e3  mov r0, #0x6
108b30c0  15e2feeb  bl 0x1086b91c
108b30c4  0600c4e5  strb r0, [r4,#0x6]
108b30c8  0600a0e1  cpy r0, r6
108b30cc  b803d0e1  ldrh r0, [r0,#0x38]
108b30d0  011c40e2  sub r1, r0, #0x100
108b30d4  fd1051e2  subs r1, r1, #0xfd
108b30d8  18009f05  ldreq r0, [0x108b30f8]
108b30dc  b20fa013  movne r0, #0x2c8
108b30e0  0410a0e1  cpy r1, r4
108b30e4  b400c4e1  strh r0, [r4,#0x4]
108b30e8  0500a0e1  cpy r0, r5
108b30ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b30f0  94d2feea  b 0x10867b48
108b30f4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
