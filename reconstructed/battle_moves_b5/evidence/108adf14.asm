108adf14  70402de9  stmdb sp!,{r4,r5,r6,lr}
108adf18  0160a0e1  cpy r6, r1
108adf1c  0240a0e1  cpy r4, r2
108adf20  0300a0e3  mov r0, #0x3
108adf24  7cf6feeb  bl 0x1086b91c
108adf28  040050e1  cmp r0, r4
108adf2c  1e00001a  bne 0x108adfac
108adf30  0010a0e3  mov r1, #0x0
108adf34  5300a0e3  mov r0, #0x53
108adf38  65defeeb  bl 0x108658d4
108adf3c  0420a0e1  cpy r2, r4
108adf40  3c10a0e3  mov r1, #0x3c
108adf44  0600a0e1  cpy r0, r6
108adf48  7e7afeeb  bl 0x1084c948
108adf4c  0400a0e1  cpy r0, r4
108adf50  c118ffeb  bl 0x1087425c
108adf54  0050a0e1  cpy r5, r0
108adf58  0010a0e1  cpy r1, r0
108adf5c  0600a0e1  cpy r0, r6
108adf60  266900eb  bl 0x108c8400
108adf64  000050e3  cmp r0, #0x0
108adf68  0050a003  moveq r5, #0x0
108adf6c  0420a0e1  cpy r2, r4
108adf70  0310a0e3  mov r1, #0x3
108adf74  0600a0e1  cpy r0, r6
108adf78  ffeffeeb  bl 0x10869f7c
108adf7c  0040a0e1  cpy r4, r0
108adf80  28209fe5  ldr r2, [0x108adfb0]
108adf84  0110a0e3  mov r1, #0x1
108adf88  040080e2  add r0, r0, #0x4
108adf8c  d07cfeeb  bl 0x1084d2d4
108adf90  0510a0e1  cpy r1, r5
108adf94  040084e2  add r0, r4, #0x4
108adf98  bf7cfeeb  bl 0x1084d29c
108adf9c  0410a0e1  cpy r1, r4
108adfa0  0600a0e1  cpy r0, r6
108adfa4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108adfa8  e6e6feea  b 0x10867b48
108adfac  7080bde8  ldmia sp!,{r4,r5,r6,pc}
