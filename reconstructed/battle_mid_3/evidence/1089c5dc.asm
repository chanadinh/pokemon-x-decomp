1089c5dc  10402de9  stmdb sp!,{r4,lr}
1089c5e0  e00090e5  ldr r0, [r0,#0xe0]
1089c5e4  0140a0e1  cpy r4, r1
1089c5e8  000050e3  cmp r0, #0x0
1089c5ec  0300000a  beq 0x1089c600
1089c5f0  9990fdeb  bl 0x1080085c
1089c5f4  0400a0e1  cpy r0, r4
1089c5f8  1040bde8  ldmia sp!,{r4,lr}
1089c5fc  bae8feea  b 0x108568ec
1089c600  1080bde8  ldmia sp!,{r4,pc}
