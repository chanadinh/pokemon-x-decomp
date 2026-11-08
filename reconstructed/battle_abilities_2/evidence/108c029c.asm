108c029c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c02a0  0150a0e1  cpy r5, r1
108c02a4  0240a0e1  cpy r4, r2
108c02a8  0200a0e3  mov r0, #0x2
108c02ac  9aadfeeb  bl 0x1086b91c
108c02b0  040050e1  cmp r0, r4
108c02b4  0700001a  bne 0x108c02d8
108c02b8  0500a0e1  cpy r0, r5
108c02bc  e52efeeb  bl 0x1084be58
108c02c0  010050e3  cmp r0, #0x1
108c02c4  0300001a  bne 0x108c02d8
108c02c8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c02cc  021aa0e3  mov r1, #0x2000
108c02d0  3600a0e3  mov r0, #0x36
108c02d4  ea32feea  b 0x1084ce84
108c02d8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
