108b6f68  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b6f6c  0160a0e1  cpy r6, r1
108b6f70  0240a0e1  cpy r4, r2
108b6f74  0300a0e3  mov r0, #0x3
108b6f78  67d2feeb  bl 0x1086b91c
108b6f7c  040050e1  cmp r0, r4
108b6f80  1b00001a  bne 0x108b6ff4
108b6f84  0410a0e1  cpy r1, r4
108b6f88  0600a0e1  cpy r0, r6
108b6f8c  b956feeb  bl 0x1084ca78
108b6f90  000050e3  cmp r0, #0x0
108b6f94  1600000a  beq 0x108b6ff4
108b6f98  0420a0e1  cpy r2, r4
108b6f9c  1f10a0e3  mov r1, #0x1f
108b6fa0  0600a0e1  cpy r0, r6
108b6fa4  f4cbfeeb  bl 0x10869f7c
108b6fa8  0050a0e1  cpy r5, r0
108b6fac  0100a0e3  mov r0, #0x1
108b6fb0  0400c5e5  strb r0, [r5,#0x4]
108b6fb4  0410a0e1  cpy r1, r4
108b6fb8  0600a0e1  cpy r0, r6
108b6fbc  65f0feeb  bl 0x10873158
108b6fc0  0500c5e5  strb r0, [r5,#0x5]
108b6fc4  0510a0e1  cpy r1, r5
108b6fc8  0600a0e1  cpy r0, r6
108b6fcc  ddc2feeb  bl 0x10867b48
108b6fd0  0420a0e1  cpy r2, r4
108b6fd4  1210a0e3  mov r1, #0x12
108b6fd8  0600a0e1  cpy r0, r6
108b6fdc  e6cbfeeb  bl 0x10869f7c
108b6fe0  0010a0e1  cpy r1, r0
108b6fe4  0440c0e5  strb r4, [r0,#0x4]
108b6fe8  0600a0e1  cpy r0, r6
108b6fec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b6ff0  d4c2feea  b 0x10867b48
108b6ff4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
