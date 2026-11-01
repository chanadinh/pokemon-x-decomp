108c0f10  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c0f14  0150a0e1  cpy r5, r1
108c0f18  0240a0e1  cpy r4, r2
108c0f1c  0400a0e3  mov r0, #0x4
108c0f20  7daafeeb  bl 0x1086b91c
108c0f24  040050e1  cmp r0, r4
108c0f28  0b00001a  bne 0x108c0f5c
108c0f2c  1b00a0e3  mov r0, #0x1b
108c0f30  79aafeeb  bl 0x1086b91c
108c0f34  010050e3  cmp r0, #0x1
108c0f38  0700001a  bne 0x108c0f5c
108c0f3c  0500a0e1  cpy r0, r5
108c0f40  632afeeb  bl 0x1084b8d4
108c0f44  010050e3  cmp r0, #0x1
108c0f48  0300001a  bne 0x108c0f5c
108c0f4c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c0f50  061ba0e3  mov r1, #0x1800
108c0f54  3600a0e3  mov r0, #0x36
108c0f58  c92ffeea  b 0x1084ce84
108c0f5c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
