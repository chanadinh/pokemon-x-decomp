10899ff4  10402de9  stmdb sp!,{r4,lr}
10899ff8  dc0090e5  ldr r0, [r0,#0xdc]
10899ffc  0140a0e1  cpy r4, r1
1089a000  000050e3  cmp r0, #0x0
1089a004  0300000a  beq 0x1089a018
1089a008  139afdeb  bl 0x1080085c
1089a00c  0400a0e1  cpy r0, r4
1089a010  1040bde8  ldmia sp!,{r4,lr}
1089a014  34f2feea  b 0x108568ec
1089a018  1080bde8  ldmia sp!,{r4,pc}
