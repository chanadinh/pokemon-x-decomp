108935cc  10402de9  stmdb sp!,{r4,lr}
108935d0  e40090e5  ldr r0, [r0,#0xe4]
108935d4  0140a0e1  cpy r4, r1
108935d8  000050e3  cmp r0, #0x0
108935dc  0300000a  beq 0x108935f0
108935e0  9db4fdeb  bl 0x1080085c
108935e4  0400a0e1  cpy r0, r4
108935e8  1040bde8  ldmia sp!,{r4,lr}
108935ec  be0cffea  b 0x108568ec
108935f0  1080bde8  ldmia sp!,{r4,pc}
