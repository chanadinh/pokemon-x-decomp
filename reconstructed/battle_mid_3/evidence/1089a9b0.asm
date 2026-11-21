1089a9b0  10402de9  stmdb sp!,{r4,lr}
1089a9b4  180190e5  ldr r0, [r0,#0x118]
1089a9b8  0140a0e1  cpy r4, r1
1089a9bc  000050e3  cmp r0, #0x0
1089a9c0  0300000a  beq 0x1089a9d4
1089a9c4  a497fdeb  bl 0x1080085c
1089a9c8  0400a0e1  cpy r0, r4
1089a9cc  1040bde8  ldmia sp!,{r4,lr}
1089a9d0  c5effeea  b 0x108568ec
1089a9d4  1080bde8  ldmia sp!,{r4,pc}
