108af958  10402de9  stmdb sp!,{r4,lr}
108af95c  0240a0e1  cpy r4, r2
108af960  0300a0e3  mov r0, #0x3
108af964  eceffeeb  bl 0x1086b91c
108af968  040050e1  cmp r0, r4
108af96c  0700001a  bne 0x108af990
108af970  5300a0e3  mov r0, #0x53
108af974  e8effeeb  bl 0x1086b91c
108af978  000050e3  cmp r0, #0x0
108af97c  0300000a  beq 0x108af990
108af980  1040bde8  ldmia sp!,{r4,lr}
108af984  0210a0e3  mov r1, #0x2
108af988  3b00a0e3  mov r0, #0x3b
108af98c  d0d7feea  b 0x108658d4
108af990  1080bde8  ldmia sp!,{r4,pc}
