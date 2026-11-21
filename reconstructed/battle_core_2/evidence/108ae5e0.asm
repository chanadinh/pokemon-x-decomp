108ae5e0  10402de9  stmdb sp!,{r4,lr}
108ae5e4  1900d0e5  ldrb r0, [r0,#0x19]
108ae5e8  0040a0e1  cpy r4, r0
108ae5ec  0410a0e1  cpy r1, r4
108ae5f0  0000a0e3  mov r0, #0x0
108ae5f4  717bfeeb  bl 0x1084d3c0
108ae5f8  000050e3  cmp r0, #0x0
108ae5fc  00f020e3  nop
108ae600  0300000a  beq 0x108ae614
108ae604  f07afeeb  bl 0x1084d1cc
108ae608  00f020e3  nop
108ae60c  00f020e3  nop
108ae610  f5ffffea  b 0x108ae5ec
108ae614  1080bde8  ldmia sp!,{r4,pc}
