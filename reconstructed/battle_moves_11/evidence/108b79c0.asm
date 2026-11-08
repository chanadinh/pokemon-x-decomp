108b79c0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b79c4  0240a0e1  cpy r4, r2
108b79c8  0200a0e3  mov r0, #0x2
108b79cc  d2cffeeb  bl 0x1086b91c
108b79d0  040050e1  cmp r0, r4
108b79d4  1000001a  bne 0x108b7a1c
108b79d8  4000a0e3  mov r0, #0x40
108b79dc  cecffeeb  bl 0x1086b91c
108b79e0  0050a0e1  cpy r5, r0
108b79e4  1200a0e3  mov r0, #0x12
108b79e8  cbcffeeb  bl 0x1086b91c
108b79ec  7040ffe6  uxth r4, r0
108b79f0  7e20a0e3  mov r2, #0x7e
108b79f4  0110a0e3  mov r1, #0x1
108b79f8  0500a0e1  cpy r0, r5
108b79fc  3456feeb  bl 0x1084d2d4
108b7a00  0410a0e1  cpy r1, r4
108b7a04  0500a0e1  cpy r0, r5
108b7a08  2356feeb  bl 0x1084d29c
108b7a0c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7a10  0110a0e3  mov r1, #0x1
108b7a14  5300a0e3  mov r0, #0x53
108b7a18  adb7feea  b 0x108658d4
108b7a1c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
