108910d4  10402de9  stmdb sp!,{r4,lr}
108910d8  f40090e5  ldr r0, [r0,#0xf4]
108910dc  0140a0e1  cpy r4, r1
108910e0  000050e3  cmp r0, #0x0
108910e4  0300000a  beq 0x108910f8
108910e8  dbbdfdeb  bl 0x1080085c
108910ec  0400a0e1  cpy r0, r4
108910f0  1040bde8  ldmia sp!,{r4,lr}
108910f4  fc15ffea  b 0x108568ec
108910f8  1080bde8  ldmia sp!,{r4,pc}
