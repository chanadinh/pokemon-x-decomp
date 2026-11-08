108c3e88  10402de9  stmdb sp!,{r4,lr}
108c3e8c  0240a0e1  cpy r4, r2
108c3e90  0300a0e3  mov r0, #0x3
108c3e94  a09efeeb  bl 0x1086b91c
108c3e98  040050e1  cmp r0, r4
108c3e9c  0900001a  bne 0x108c3ec8
108c3ea0  1200a0e3  mov r0, #0x12
108c3ea4  9c9efeeb  bl 0x1086b91c
108c3ea8  7000ffe6  uxth r0, r0
108c3eac  771afeeb  bl 0x1084a890
108c3eb0  000050e3  cmp r0, #0x0
108c3eb4  0300000a  beq 0x108c3ec8
108c3eb8  1040bde8  ldmia sp!,{r4,lr}
108c3ebc  0110a0e3  mov r1, #0x1
108c3ec0  4900a0e3  mov r0, #0x49
108c3ec4  8286feea  b 0x108658d4
108c3ec8  1080bde8  ldmia sp!,{r4,pc}
