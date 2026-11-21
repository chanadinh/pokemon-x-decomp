1088bcb8  10402de9  stmdb sp!,{r4,lr}
1088bcbc  ec0090e5  ldr r0, [r0,#0xec]
1088bcc0  0140a0e1  cpy r4, r1
1088bcc4  000050e3  cmp r0, #0x0
1088bcc8  0300000a  beq 0x1088bcdc
1088bccc  e2d2fdeb  bl 0x1080085c
1088bcd0  0400a0e1  cpy r0, r4
1088bcd4  1040bde8  ldmia sp!,{r4,lr}
1088bcd8  032bffea  b 0x108568ec
1088bcdc  1080bde8  ldmia sp!,{r4,pc}
