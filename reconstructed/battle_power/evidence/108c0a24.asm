108c0a24  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c0a28  0350a0e1  cpy r5, r3
108c0a2c  0240a0e1  cpy r4, r2
108c0a30  0300a0e3  mov r0, #0x3
108c0a34  b8abfeeb  bl 0x1086b91c
108c0a38  040050e1  cmp r0, r4
108c0a3c  0a00001a  bne 0x108c0a6c
108c0a40  1600a0e3  mov r0, #0x16
108c0a44  b4abfeeb  bl 0x1086b91c
108c0a48  020050e3  cmp r0, #0x2
108c0a4c  0600001a  bne 0x108c0a6c
108c0a50  000095e5  ldr r0, [r5,#0x0]
108c0a54  000050e3  cmp r0, #0x0
108c0a58  0300000a  beq 0x108c0a6c
108c0a5c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c0a60  3200a0e3  mov r0, #0x32
108c0a64  04109fe5  ldr r1, [0x108c0a70]
108c0a68  0531feea  b 0x1084ce84
108c0a6c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
