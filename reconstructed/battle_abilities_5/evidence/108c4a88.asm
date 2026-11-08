108c4a88  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c4a8c  0160a0e1  cpy r6, r1
108c4a90  0250a0e1  cpy r5, r2
108c4a94  0200a0e3  mov r0, #0x2
108c4a98  9f9bfeeb  bl 0x1086b91c
108c4a9c  050050e1  cmp r0, r5
108c4aa0  2b00001a  bne 0x108c4b54
108c4aa4  0110a0e3  mov r1, #0x1
108c4aa8  5300a0e3  mov r0, #0x53
108c4aac  8883feeb  bl 0x108658d4
108c4ab0  000050e3  cmp r0, #0x0
108c4ab4  2600000a  beq 0x108c4b54
108c4ab8  0520a0e1  cpy r2, r5
108c4abc  0110a0e3  mov r1, #0x1
108c4ac0  0600a0e1  cpy r0, r6
108c4ac4  9f1ffeeb  bl 0x1084c948
108c4ac8  0200a0e3  mov r0, #0x2
108c4acc  929bfeeb  bl 0x1086b91c
108c4ad0  050050e1  cmp r0, r5
108c4ad4  1900001a  bne 0x108c4b40
108c4ad8  0510a0e1  cpy r1, r5
108c4adc  0600a0e1  cpy r0, r6
108c4ae0  6e3efeeb  bl 0x108544a0
108c4ae4  461bfeeb  bl 0x1084b804
108c4ae8  000050e3  cmp r0, #0x0
108c4aec  1300001a  bne 0x108c4b40
108c4af0  0520a0e1  cpy r2, r5
108c4af4  0310a0e3  mov r1, #0x3
108c4af8  0600a0e1  cpy r0, r6
108c4afc  1e95feeb  bl 0x10869f7c
108c4b00  0040a0e1  cpy r4, r0
108c4b04  bf2fa0e3  mov r2, #0x2fc
108c4b08  0210a0e3  mov r1, #0x2
108c4b0c  040080e2  add r0, r0, #0x4
108c4b10  ef21feeb  bl 0x1084d2d4
108c4b14  0510a0e1  cpy r1, r5
108c4b18  040084e2  add r0, r4, #0x4
108c4b1c  de21feeb  bl 0x1084d29c
108c4b20  1200a0e3  mov r0, #0x12
108c4b24  7c9bfeeb  bl 0x1086b91c
108c4b28  0010a0e1  cpy r1, r0
108c4b2c  040084e2  add r0, r4, #0x4
108c4b30  d921feeb  bl 0x1084d29c
108c4b34  0410a0e1  cpy r1, r4
108c4b38  0600a0e1  cpy r0, r6
108c4b3c  018cfeeb  bl 0x10867b48
108c4b40  0520a0e1  cpy r2, r5
108c4b44  0600a0e1  cpy r0, r6
108c4b48  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c4b4c  0210a0e3  mov r1, #0x2
108c4b50  7c1ffeea  b 0x1084c948
108c4b54  7080bde8  ldmia sp!,{r4,r5,r6,pc}
