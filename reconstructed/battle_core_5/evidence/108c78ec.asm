108c78ec  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c78f0  0a0051e3  cmp r1, #0xa
108c78f4  0050a0e1  cpy r5, r0
108c78f8  0140a0e1  cpy r4, r1
108c78fc  0300003a  bcc 0x108c7910
108c7900  0020a0e3  mov r2, #0x0
108c7904  0210a0e1  cpy r1, r2
108c7908  0200a0e1  cpy r0, r2
108c790c  a0e2fceb  bl 0x10800394
108c7910  040085e0  add r0, r5, r4
108c7914  ec07d0e5  ldrb r0, [r0,#0x7ec]
108c7918  7080bde8  ldmia sp!,{r4,r5,r6,pc}
