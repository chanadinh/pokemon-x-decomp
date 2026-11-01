108c3134  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108c3138  0140a0e1  cpy r4, r1
108c313c  0250a0e1  cpy r5, r2
108c3140  0360a0e1  cpy r6, r3
108c3144  0200a0e3  mov r0, #0x2
108c3148  f3a1feeb  bl 0x1086b91c
108c314c  ff1000e2  and r1, r0, #0xff
108c3150  14009fe5  ldr r0, [0x108c316c]
108c3154  0630a0e1  cpy r3, r6
108c3158  0520a0e1  cpy r2, r5
108c315c  00008de5  str r0, [sp,#0x0]
108c3160  0400a0e1  cpy r0, r4
108c3164  1a1efeeb  bl 0x1084a9d4
108c3168  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
