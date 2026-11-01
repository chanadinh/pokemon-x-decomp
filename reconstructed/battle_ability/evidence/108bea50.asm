108bea50  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bea54  0150a0e1  cpy r5, r1
108bea58  0240a0e1  cpy r4, r2
108bea5c  0300a0e3  mov r0, #0x3
108bea60  adb3feeb  bl 0x1086b91c
108bea64  040050e1  cmp r0, r4
108bea68  1300001a  bne 0x108beabc
108bea6c  0410a0e1  cpy r1, r4
108bea70  0500a0e1  cpy r0, r5
108bea74  8956feeb  bl 0x108544a0
108bea78  0050a0e1  cpy r5, r0
108bea7c  0310a0e3  mov r1, #0x3
108bea80  a735feeb  bl 0x1084c124
108bea84  0040a0e1  cpy r4, r0
108bea88  0d10a0e3  mov r1, #0xd
108bea8c  0500a0e1  cpy r0, r5
108bea90  62d6feeb  bl 0x10874420
108bea94  040050e1  cmp r0, r4
108bea98  0700008a  bhi 0x108beabc
108bea9c  1600a0e3  mov r0, #0x16
108beaa0  9db3feeb  bl 0x1086b91c
108beaa4  0b0050e3  cmp r0, #0xb
108beaa8  0300001a  bne 0x108beabc
108beaac  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108beab0  061ba0e3  mov r1, #0x1800
108beab4  3600a0e3  mov r0, #0x36
108beab8  f138feea  b 0x1084ce84
108beabc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
