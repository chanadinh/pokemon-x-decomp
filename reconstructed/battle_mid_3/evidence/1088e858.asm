1088e858  10402de9  stmdb sp!,{r4,lr}
1088e85c  e80090e5  ldr r0, [r0,#0xe8]
1088e860  0140a0e1  cpy r4, r1
1088e864  000050e3  cmp r0, #0x0
1088e868  0300000a  beq 0x1088e87c
1088e86c  fac7fdeb  bl 0x1080085c
1088e870  0400a0e1  cpy r0, r4
1088e874  1040bde8  ldmia sp!,{r4,lr}
1088e878  1b20ffea  b 0x108568ec
1088e87c  1080bde8  ldmia sp!,{r4,pc}
