1088b2e8  10402de9  stmdb sp!,{r4,lr}
1088b2ec  e80090e5  ldr r0, [r0,#0xe8]
1088b2f0  0140a0e1  cpy r4, r1
1088b2f4  000050e3  cmp r0, #0x0
1088b2f8  0300000a  beq 0x1088b30c
1088b2fc  56d5fdeb  bl 0x1080085c
1088b300  0400a0e1  cpy r0, r4
1088b304  1040bde8  ldmia sp!,{r4,lr}
1088b308  772dffea  b 0x108568ec
1088b30c  1080bde8  ldmia sp!,{r4,pc}
