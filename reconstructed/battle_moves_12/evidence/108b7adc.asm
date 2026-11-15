108b7adc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b7ae0  0150a0e1  cpy r5, r1
108b7ae4  0310a0e3  mov r1, #0x3
108b7ae8  0500a0e1  cpy r0, r5
108b7aec  22c9feeb  bl 0x10869f7c
108b7af0  0040a0e1  cpy r4, r0
108b7af4  7320a0e3  mov r2, #0x73
108b7af8  0110a0e3  mov r1, #0x1
108b7afc  040080e2  add r0, r0, #0x4
108b7b00  f355feeb  bl 0x1084d2d4
108b7b04  0410a0e1  cpy r1, r4
108b7b08  0500a0e1  cpy r0, r5
108b7b0c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7b10  0cc0feea  b 0x10867b48
