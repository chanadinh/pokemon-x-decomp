108c2b50  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2b54  0150a0e1  cpy r5, r1
108c2b58  0240a0e1  cpy r4, r2
108c2b5c  0460a0e3  mov r6, #0x4
108c2b60  0200a0e3  mov r0, #0x2
108c2b64  6ca3feeb  bl 0x1086b91c
108c2b68  040050e1  cmp r0, r4
108c2b6c  0400001a  bne 0x108c2b84
108c2b70  0620a0e1  cpy r2, r6
108c2b74  0410a0e1  cpy r1, r4
108c2b78  0500a0e1  cpy r0, r5
108c2b7c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c2b80  0720feea  b 0x1084aba4
108c2b84  7080bde8  ldmia sp!,{r4,r5,r6,pc}
