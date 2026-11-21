10892c34  70402de9  stmdb sp!,{r4,r5,r6,lr}
10892c38  0040a0e1  cpy r4, r0
10892c3c  0000a0e3  mov r0, #0x0
10892c40  2400c4e5  strb r0, [r4,#0x24]
10892c44  0300a0e3  mov r0, #0x3
10892c48  0060a0e1  cpy r6, r0
10892c4c  d900c4e5  strb r0, [r4,#0xd9]
10892c50  ec0094e5  ldr r0, [r4,#0xec]
10892c54  0050a0e3  mov r5, #0x0
10892c58  45bafdeb  bl 0x10801574
10892c5c  f450c4e5  strb r5, [r4,#0xf4]
10892c60  4110a0e3  mov r1, #0x41
10892c64  f560c4e5  strb r6, [r4,#0xf5]
10892c68  0400a0e1  cpy r0, r4
10892c6c  f610c4e5  strb r1, [r4,#0xf6]
10892c70  85fafeeb  bl 0x1085168c
10892c74  e40094e5  ldr r0, [r4,#0xe4]
10892c78  f5bbfdeb  bl 0x10801c54
10892c7c  0050a0e1  cpy r5, r0
10892c80  e40094e5  ldr r0, [r4,#0xe4]
10892c84  0010a0e3  mov r1, #0x0
10892c88  f3b6fdeb  bl 0x1080085c
10892c8c  e40094e5  ldr r0, [r4,#0xe4]
10892c90  e1bbfdeb  bl 0x10801c1c
10892c94  e40094e5  ldr r0, [r4,#0xe4]
10892c98  0510a0e1  cpy r1, r5
10892c9c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
10892ca0  edb6fdea  b 0x1080085c
