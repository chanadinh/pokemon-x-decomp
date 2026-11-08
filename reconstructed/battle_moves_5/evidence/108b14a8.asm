108b14a8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b14ac  0150a0e1  cpy r5, r1
108b14b0  0240a0e1  cpy r4, r2
108b14b4  0300a0e3  mov r0, #0x3
108b14b8  17e9feeb  bl 0x1086b91c
108b14bc  040050e1  cmp r0, r4
108b14c0  1000001a  bne 0x108b1508
108b14c4  0420a0e1  cpy r2, r4
108b14c8  1c10a0e3  mov r1, #0x1c
108b14cc  0500a0e1  cpy r0, r5
108b14d0  a9e2feeb  bl 0x10869f7c
108b14d4  0040a0e1  cpy r4, r0
108b14d8  0500a0e3  mov r0, #0x5
108b14dc  0400c4e5  strb r0, [r4,#0x4]
108b14e0  556afeeb  bl 0x1084be3c
108b14e4  080084e5  str r0, [r4,#0x8]
108b14e8  7820a0e3  mov r2, #0x78
108b14ec  0110a0e3  mov r1, #0x1
108b14f0  0c0084e2  add r0, r4, #0xc
108b14f4  766ffeeb  bl 0x1084d2d4
108b14f8  0410a0e1  cpy r1, r4
108b14fc  0500a0e1  cpy r0, r5
108b1500  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b1504  8fd9feea  b 0x10867b48
108b1508  7080bde8  ldmia sp!,{r4,r5,r6,pc}
