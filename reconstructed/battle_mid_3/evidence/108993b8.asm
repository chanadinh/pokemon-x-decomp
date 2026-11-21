108993b8  10402de9  stmdb sp!,{r4,lr}
108993bc  e80090e5  ldr r0, [r0,#0xe8]
108993c0  0140a0e1  cpy r4, r1
108993c4  000050e3  cmp r0, #0x0
108993c8  0300000a  beq 0x108993dc
108993cc  229dfdeb  bl 0x1080085c
108993d0  0400a0e1  cpy r0, r4
108993d4  1040bde8  ldmia sp!,{r4,lr}
108993d8  43f5feea  b 0x108568ec
108993dc  1080bde8  ldmia sp!,{r4,pc}
