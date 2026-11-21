1088db3c  10402de9  stmdb sp!,{r4,lr}
1088db40  d80090e5  ldr r0, [r0,#0xd8]
1088db44  0140a0e1  cpy r4, r1
1088db48  000050e3  cmp r0, #0x0
1088db4c  0300000a  beq 0x1088db60
1088db50  41cbfdeb  bl 0x1080085c
1088db54  0400a0e1  cpy r0, r4
1088db58  1040bde8  ldmia sp!,{r4,lr}
1088db5c  6223ffea  b 0x108568ec
1088db60  1080bde8  ldmia sp!,{r4,pc}
