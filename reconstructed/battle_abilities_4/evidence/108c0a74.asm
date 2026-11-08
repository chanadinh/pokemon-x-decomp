108c0a74  10402de9  stmdb sp!,{r4,lr}
108c0a78  0240a0e1  cpy r4, r2
108c0a7c  0200a0e3  mov r0, #0x2
108c0a80  a5abfeeb  bl 0x1086b91c
108c0a84  040050e1  cmp r0, r4
108c0a88  0700001a  bne 0x108c0aac
108c0a8c  4500a0e3  mov r0, #0x45
108c0a90  a1abfeeb  bl 0x1086b91c
108c0a94  000050e3  cmp r0, #0x0
108c0a98  0300000a  beq 0x108c0aac
108c0a9c  1040bde8  ldmia sp!,{r4,lr}
108c0aa0  021aa0e3  mov r1, #0x2000
108c0aa4  3600a0e3  mov r0, #0x36
108c0aa8  8993feea  b 0x108658d4
108c0aac  1080bde8  ldmia sp!,{r4,pc}
