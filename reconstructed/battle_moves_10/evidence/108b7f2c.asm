108b7f2c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b7f30  0150a0e1  cpy r5, r1
108b7f34  0240a0e1  cpy r4, r2
108b7f38  0200a0e3  mov r0, #0x2
108b7f3c  76cefeeb  bl 0x1086b91c
108b7f40  040050e1  cmp r0, r4
108b7f44  2e00001a  bne 0x108b8004
108b7f48  2300a0e3  mov r0, #0x23
108b7f4c  72cefeeb  bl 0x1086b91c
108b7f50  ff0010e3  tst r0, #0xff
108b7f54  2a00001a  bne 0x108b8004
108b7f58  0410a0e1  cpy r1, r4
108b7f5c  0500a0e1  cpy r0, r5
108b7f60  4e71feeb  bl 0x108544a0
108b7f64  0040a0e1  cpy r4, r0
108b7f68  0210a0e3  mov r1, #0x2
108b7f6c  c57afeeb  bl 0x10856a88
108b7f70  000050e3  cmp r0, #0x0
108b7f74  0300000a  beq 0x108b7f88
108b7f78  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7f7c  1b10a0e3  mov r1, #0x1b
108b7f80  2300a0e3  mov r0, #0x23
108b7f84  52b6feea  b 0x108658d4
108b7f88  0f10a0e3  mov r1, #0xf
108b7f8c  0400a0e1  cpy r0, r4
108b7f90  bc7afeeb  bl 0x10856a88
108b7f94  000050e3  cmp r0, #0x0
108b7f98  00f020e3  nop
108b7f9c  0300000a  beq 0x108b7fb0
108b7fa0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7fa4  0d10a0e3  mov r1, #0xd
108b7fa8  2300a0e3  mov r0, #0x23
108b7fac  48b6feea  b 0x108658d4
108b7fb0  0400a0e1  cpy r0, r4
108b7fb4  d754feeb  bl 0x1084d318
108b7fb8  000050e3  cmp r0, #0x0
108b7fbc  00f020e3  nop
108b7fc0  0300000a  beq 0x108b7fd4
108b7fc4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7fc8  0e10a0e3  mov r1, #0xe
108b7fcc  2300a0e3  mov r0, #0x23
108b7fd0  3fb6feea  b 0x108658d4
108b7fd4  1110a0e3  mov r1, #0x11
108b7fd8  0400a0e1  cpy r0, r4
108b7fdc  0ff1feeb  bl 0x10874420
108b7fe0  7000ffe6  uxth r0, r0
108b7fe4  0f0050e3  cmp r0, #0xf
108b7fe8  0100000a  beq 0x108b7ff4
108b7fec  480050e3  cmp r0, #0x48
108b7ff0  0300001a  bne 0x108b8004
108b7ff4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b7ff8  0f10a0e3  mov r1, #0xf
108b7ffc  2300a0e3  mov r0, #0x23
108b8000  33b6feea  b 0x108658d4
108b8004  7080bde8  ldmia sp!,{r4,r5,r6,pc}
