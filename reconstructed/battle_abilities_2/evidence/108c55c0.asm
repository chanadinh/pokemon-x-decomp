108c55c0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c55c4  0150a0e1  cpy r5, r1
108c55c8  0240a0e1  cpy r4, r2
108c55cc  0200a0e3  mov r0, #0x2
108c55d0  d198feeb  bl 0x1086b91c
108c55d4  040050e1  cmp r0, r4
108c55d8  0700001a  bne 0x108c55fc
108c55dc  0500a0e1  cpy r0, r5
108c55e0  1c1afeeb  bl 0x1084be58
108c55e4  010050e3  cmp r0, #0x1
108c55e8  0300001a  bne 0x108c55fc
108c55ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c55f0  0110a0e3  mov r1, #0x1
108c55f4  4200a0e3  mov r0, #0x42
108c55f8  b580feea  b 0x108658d4
108c55fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
