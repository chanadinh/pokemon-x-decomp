108939f0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108939f4  0040a0e1  cpy r4, r0
108939f8  0000a0e3  mov r0, #0x0
108939fc  2400c4e5  strb r0, [r4,#0x24]
10893a00  0300a0e3  mov r0, #0x3
10893a04  0060a0e1  cpy r6, r0
10893a08  d900c4e5  strb r0, [r4,#0xd9]
10893a0c  ec0094e5  ldr r0, [r4,#0xec]
10893a10  0050a0e3  mov r5, #0x0
10893a14  d6b6fdeb  bl 0x10801574
10893a18  f450c4e5  strb r5, [r4,#0xf4]
10893a1c  4110a0e3  mov r1, #0x41
10893a20  f560c4e5  strb r6, [r4,#0xf5]
10893a24  0400a0e1  cpy r0, r4
10893a28  f610c4e5  strb r1, [r4,#0xf6]
10893a2c  e5f5feeb  bl 0x108511c8
10893a30  e40094e5  ldr r0, [r4,#0xe4]
10893a34  86b8fdeb  bl 0x10801c54
10893a38  0050a0e1  cpy r5, r0
10893a3c  e40094e5  ldr r0, [r4,#0xe4]
10893a40  0010a0e3  mov r1, #0x0
10893a44  84b3fdeb  bl 0x1080085c
10893a48  e40094e5  ldr r0, [r4,#0xe4]
10893a4c  72b8fdeb  bl 0x10801c1c
10893a50  e40094e5  ldr r0, [r4,#0xe4]
10893a54  0510a0e1  cpy r1, r5
10893a58  7040bde8  ldmia sp!,{r4,r5,r6,lr}
10893a5c  7eb3fdea  b 0x1080085c
