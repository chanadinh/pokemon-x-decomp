10898a04  10402de9  stmdb sp!,{r4,lr}
10898a08  e80090e5  ldr r0, [r0,#0xe8]
10898a0c  0140a0e1  cpy r4, r1
10898a10  000050e3  cmp r0, #0x0
10898a14  0300000a  beq 0x10898a28
10898a18  8f9ffdeb  bl 0x1080085c
10898a1c  0400a0e1  cpy r0, r4
10898a20  1040bde8  ldmia sp!,{r4,lr}
10898a24  b0f7feea  b 0x108568ec
10898a28  1080bde8  ldmia sp!,{r4,pc}
