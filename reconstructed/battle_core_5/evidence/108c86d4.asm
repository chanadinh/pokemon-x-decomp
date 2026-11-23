108c86d4  030051e3  cmp r1, #0x3
108c86d8  10402de9  stmdb sp!,{r4,lr}
108c86dc  0600002a  bcs 0x108c86fc
108c86e0  0522d0e5  ldrb r2, [r0,#0x205]
108c86e4  011042e0  sub r1, r2, r1
108c86e8  000051e3  cmp r1, #0x0
108c86ec  031081b2  addlt r1, r1, #0x3
108c86f0  010080e0  add r0, r0, r1
108c86f4  0202d0e5  ldrb r0, [r0,#0x202]
108c86f8  1080bde8  ldmia sp!,{r4,pc}
108c86fc  0020a0e3  mov r2, #0x0
108c8700  0210a0e1  cpy r1, r2
108c8704  0200a0e1  cpy r0, r2
108c8708  21dffceb  bl 0x10800394
108c870c  0000a0e3  mov r0, #0x0
108c8710  1080bde8  ldmia sp!,{r4,pc}
