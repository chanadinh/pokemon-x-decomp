1088f210  10402de9  stmdb sp!,{r4,lr}
1088f214  e00090e5  ldr r0, [r0,#0xe0]
1088f218  0140a0e1  cpy r4, r1
1088f21c  000050e3  cmp r0, #0x0
1088f220  0300000a  beq 0x1088f234
1088f224  8cc5fdeb  bl 0x1080085c
1088f228  0400a0e1  cpy r0, r4
1088f22c  1040bde8  ldmia sp!,{r4,lr}
1088f230  ad1dffea  b 0x108568ec
1088f234  1080bde8  ldmia sp!,{r4,pc}
