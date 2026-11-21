108b9e14  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9e18  0160a0e1  cpy r6, r1
108b9e1c  0250a0e1  cpy r5, r2
108b9e20  0300a0e3  mov r0, #0x3
108b9e24  bcc6feeb  bl 0x1086b91c
108b9e28  050050e1  cmp r0, r5
108b9e2c  0f00001a  bne 0x108b9e70
108b9e30  0520a0e1  cpy r2, r5
108b9e34  0310a0e3  mov r1, #0x3
108b9e38  0600a0e1  cpy r0, r6
108b9e3c  4ec0feeb  bl 0x10869f7c
108b9e40  0040a0e1  cpy r4, r0
108b9e44  28209fe5  ldr r2, [0x108b9e74]
108b9e48  0210a0e3  mov r1, #0x2
108b9e4c  040080e2  add r0, r0, #0x4
108b9e50  1f4dfeeb  bl 0x1084d2d4
108b9e54  0510a0e1  cpy r1, r5
108b9e58  040084e2  add r0, r4, #0x4
108b9e5c  0e4dfeeb  bl 0x1084d29c
108b9e60  0410a0e1  cpy r1, r4
108b9e64  0600a0e1  cpy r0, r6
108b9e68  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9e6c  35b7feea  b 0x10867b48
108b9e70  7080bde8  ldmia sp!,{r4,r5,r6,pc}
