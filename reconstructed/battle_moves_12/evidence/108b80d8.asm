108b80d8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b80dc  0150a0e1  cpy r5, r1
108b80e0  0310a0e3  mov r1, #0x3
108b80e4  0500a0e1  cpy r0, r5
108b80e8  a3c7feeb  bl 0x10869f7c
108b80ec  0040a0e1  cpy r4, r0
108b80f0  7420a0e3  mov r2, #0x74
108b80f4  0110a0e3  mov r1, #0x1
108b80f8  040080e2  add r0, r0, #0x4
108b80fc  7454feeb  bl 0x1084d2d4
108b8100  0410a0e1  cpy r1, r4
108b8104  0500a0e1  cpy r0, r5
108b8108  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b810c  8dbefeea  b 0x10867b48
