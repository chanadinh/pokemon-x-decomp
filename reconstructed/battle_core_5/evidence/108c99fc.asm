108c99fc  040051e3  cmp r1, #0x4
108c9a00  10402de9  stmdb sp!,{r4,lr}
108c9a04  0300002a  bcs 0x108c9a18
108c9a08  012281e0  add r2, r1, r1, lsl #0x4
108c9a0c  811282e0  add r1, r2, r1, lsl #0x5
108c9a10  0100d0e7  ldrb r0, [r0,r1]
108c9a14  1080bde8  ldmia sp!,{r4,pc}
108c9a18  0020a0e3  mov r2, #0x0
108c9a1c  0210a0e1  cpy r1, r2
108c9a20  0200a0e1  cpy r0, r2
108c9a24  5adafceb  bl 0x10800394
108c9a28  0000a0e3  mov r0, #0x0
108c9a2c  1080bde8  ldmia sp!,{r4,pc}
