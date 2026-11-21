1088fa38  10402de9  stmdb sp!,{r4,lr}
1088fa3c  e00090e5  ldr r0, [r0,#0xe0]
1088fa40  0140a0e1  cpy r4, r1
1088fa44  000050e3  cmp r0, #0x0
1088fa48  0300000a  beq 0x1088fa5c
1088fa4c  82c3fdeb  bl 0x1080085c
1088fa50  0400a0e1  cpy r0, r4
1088fa54  1040bde8  ldmia sp!,{r4,lr}
1088fa58  a31bffea  b 0x108568ec
1088fa5c  1080bde8  ldmia sp!,{r4,pc}
