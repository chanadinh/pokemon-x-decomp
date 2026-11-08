108b1ae0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b1ae4  0150a0e1  cpy r5, r1
108b1ae8  0240a0e1  cpy r4, r2
108b1aec  0300a0e3  mov r0, #0x3
108b1af0  89e7feeb  bl 0x1086b91c
108b1af4  040050e1  cmp r0, r4
108b1af8  1000001a  bne 0x108b1b40
108b1afc  0120a0e3  mov r2, #0x1
108b1b00  0410a0e1  cpy r1, r4
108b1b04  0500a0e1  cpy r0, r5
108b1b08  2f6afeeb  bl 0x1084c3cc
108b1b0c  0420a0e1  cpy r2, r4
108b1b10  0310a0e3  mov r1, #0x3
108b1b14  0500a0e1  cpy r0, r5
108b1b18  17e1feeb  bl 0x10869f7c
108b1b1c  0040a0e1  cpy r4, r0
108b1b20  ff20a0e3  mov r2, #0xff
108b1b24  0110a0e3  mov r1, #0x1
108b1b28  040080e2  add r0, r0, #0x4
108b1b2c  e86dfeeb  bl 0x1084d2d4
108b1b30  0410a0e1  cpy r1, r4
108b1b34  0500a0e1  cpy r0, r5
108b1b38  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b1b3c  01d8feea  b 0x10867b48
108b1b40  7080bde8  ldmia sp!,{r4,r5,r6,pc}
