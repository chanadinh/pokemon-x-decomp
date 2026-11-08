108c43a0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c43a4  0250a0e1  cpy r5, r2
108c43a8  0500a0e3  mov r0, #0x5
108c43ac  5a9dfeeb  bl 0x1086b91c
108c43b0  006050e2  subs r6, r0, #0x0
108c43b4  0040a0e3  mov r4, #0x0
108c43b8  1100009a  bls 0x108c4404
108c43bc  060084e2  add r0, r4, #0x6
108c43c0  ff0000e2  and r0, r0, #0xff
108c43c4  549dfeeb  bl 0x1086b91c
108c43c8  050050e1  cmp r0, r5
108c43cc  00f020e3  nop
108c43d0  0800001a  bne 0x108c43f8
108c43d4  0500a0e3  mov r0, #0x5
108c43d8  4f9dfeeb  bl 0x1086b91c
108c43dc  010050e3  cmp r0, #0x1
108c43e0  00f020e3  nop
108c43e4  0600001a  bne 0x108c4404
108c43e8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c43ec  0110a0e3  mov r1, #0x1
108c43f0  4800a0e3  mov r0, #0x48
108c43f4  3685feea  b 0x108658d4
108c43f8  014084e2  add r4, r4, #0x1
108c43fc  040056e1  cmp r6, r4
108c4400  edffff8a  bhi 0x108c43bc
108c4404  7080bde8  ldmia sp!,{r4,r5,r6,pc}
