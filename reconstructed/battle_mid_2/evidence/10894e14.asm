10894e14  70402de9  stmdb sp!,{r4,r5,r6,lr}
10894e18  0040a0e1  cpy r4, r0
10894e1c  0000a0e3  mov r0, #0x0
10894e20  2400c4e5  strb r0, [r4,#0x24]
10894e24  0300a0e3  mov r0, #0x3
10894e28  0060a0e1  cpy r6, r0
10894e2c  d900c4e5  strb r0, [r4,#0xd9]
10894e30  ec0094e5  ldr r0, [r4,#0xec]
10894e34  0050a0e3  mov r5, #0x0
10894e38  cdb1fdeb  bl 0x10801574
10894e3c  f450c4e5  strb r5, [r4,#0xf4]
10894e40  5f10a0e3  mov r1, #0x5f
10894e44  f560c4e5  strb r6, [r4,#0xf5]
10894e48  0400a0e1  cpy r0, r4
10894e4c  f610c4e5  strb r1, [r4,#0xf6]
10894e50  a7effeeb  bl 0x10850cf4
10894e54  e40094e5  ldr r0, [r4,#0xe4]
10894e58  7db3fdeb  bl 0x10801c54
10894e5c  0050a0e1  cpy r5, r0
10894e60  e40094e5  ldr r0, [r4,#0xe4]
10894e64  0010a0e3  mov r1, #0x0
10894e68  7baefdeb  bl 0x1080085c
10894e6c  e40094e5  ldr r0, [r4,#0xe4]
10894e70  69b3fdeb  bl 0x10801c1c
10894e74  e40094e5  ldr r0, [r4,#0xe4]
10894e78  0510a0e1  cpy r1, r5
10894e7c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
10894e80  75aefdea  b 0x1080085c
