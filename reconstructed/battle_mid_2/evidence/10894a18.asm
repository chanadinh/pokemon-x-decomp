10894a18  70402de9  stmdb sp!,{r4,r5,r6,lr}
10894a1c  0040a0e1  cpy r4, r0
10894a20  0000a0e3  mov r0, #0x0
10894a24  2400c4e5  strb r0, [r4,#0x24]
10894a28  0300a0e3  mov r0, #0x3
10894a2c  0060a0e1  cpy r6, r0
10894a30  d900c4e5  strb r0, [r4,#0xd9]
10894a34  ec0094e5  ldr r0, [r4,#0xec]
10894a38  0050a0e3  mov r5, #0x0
10894a3c  ccb2fdeb  bl 0x10801574
10894a40  f450c4e5  strb r5, [r4,#0xf4]
10894a44  5f10a0e3  mov r1, #0x5f
10894a48  f560c4e5  strb r6, [r4,#0xf5]
10894a4c  0400a0e1  cpy r0, r4
10894a50  f610c4e5  strb r1, [r4,#0xf6]
10894a54  a6f0feeb  bl 0x10850cf4
10894a58  ec0094e5  ldr r0, [r4,#0xec]
10894a5c  0150a0e3  mov r5, #0x1
10894a60  0260a0e3  mov r6, #0x2
10894a64  c4b2fdeb  bl 0x1080157c
10894a68  f450c4e5  strb r5, [r4,#0xf4]
10894a6c  0200a0e3  mov r0, #0x2
10894a70  f560c4e5  strb r6, [r4,#0xf5]
10894a74  d900c4e5  strb r0, [r4,#0xd9]
10894a78  7080bde8  ldmia sp!,{r4,r5,r6,pc}
