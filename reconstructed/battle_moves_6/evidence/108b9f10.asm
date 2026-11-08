108b9f10  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b9f14  0160a0e1  cpy r6, r1
108b9f18  0240a0e1  cpy r4, r2
108b9f1c  0300a0e3  mov r0, #0x3
108b9f20  7dc6feeb  bl 0x1086b91c
108b9f24  040050e1  cmp r0, r4
108b9f28  1900001a  bne 0x108b9f94
108b9f2c  0420a0e1  cpy r2, r4
108b9f30  1710a0e3  mov r1, #0x17
108b9f34  0600a0e1  cpy r0, r6
108b9f38  0fc0feeb  bl 0x10869f7c
108b9f3c  0010a0e1  cpy r1, r0
108b9f40  0500a0e3  mov r0, #0x5
108b9f44  0540c1e5  strb r4, [r1,#0x5]
108b9f48  0400c1e5  strb r0, [r1,#0x4]
108b9f4c  0600a0e1  cpy r0, r6
108b9f50  fcb6feeb  bl 0x10867b48
108b9f54  0420a0e1  cpy r2, r4
108b9f58  0310a0e3  mov r1, #0x3
108b9f5c  0600a0e1  cpy r0, r6
108b9f60  05c0feeb  bl 0x10869f7c
108b9f64  0050a0e1  cpy r5, r0
108b9f68  28209fe5  ldr r2, [0x108b9f98]
108b9f6c  0210a0e3  mov r1, #0x2
108b9f70  040080e2  add r0, r0, #0x4
108b9f74  d64cfeeb  bl 0x1084d2d4
108b9f78  0410a0e1  cpy r1, r4
108b9f7c  040085e2  add r0, r5, #0x4
108b9f80  c54cfeeb  bl 0x1084d29c
108b9f84  0510a0e1  cpy r1, r5
108b9f88  0600a0e1  cpy r0, r6
108b9f8c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9f90  ecb6feea  b 0x10867b48
108b9f94  7080bde8  ldmia sp!,{r4,r5,r6,pc}
