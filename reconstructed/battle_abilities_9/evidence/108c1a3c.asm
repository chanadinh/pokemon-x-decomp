108c1a3c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c1a40  0050a0e1  cpy r5, r0
108c1a44  0240a0e1  cpy r4, r2
108c1a48  0200a0e3  mov r0, #0x2
108c1a4c  b2a7feeb  bl 0x1086b91c
108c1a50  040050e1  cmp r0, r4
108c1a54  0200001a  bne 0x108c1a64
108c1a58  0500a0e1  cpy r0, r5
108c1a5c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c1a60  2125feea  b 0x1084aeec
108c1a64  7080bde8  ldmia sp!,{r4,r5,r6,pc}
