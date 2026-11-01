108c3c3c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c3c40  0160a0e1  cpy r6, r1
108c3c44  0240a0e1  cpy r4, r2
108c3c48  70709fe5  ldr r7, [0x108c3cc0]
108c3c4c  0200a0e3  mov r0, #0x2
108c3c50  319ffeeb  bl 0x1086b91c
108c3c54  040050e1  cmp r0, r4
108c3c58  1700001a  bne 0x108c3cbc
108c3c5c  0420a0e1  cpy r2, r4
108c3c60  0110a0e3  mov r1, #0x1
108c3c64  0600a0e1  cpy r0, r6
108c3c68  3623feeb  bl 0x1084c948
108c3c6c  0420a0e1  cpy r2, r4
108c3c70  0310a0e3  mov r1, #0x3
108c3c74  0600a0e1  cpy r0, r6
108c3c78  bf98feeb  bl 0x10869f7c
108c3c7c  0050a0e1  cpy r5, r0
108c3c80  0720a0e1  cpy r2, r7
108c3c84  0210a0e3  mov r1, #0x2
108c3c88  040080e2  add r0, r0, #0x4
108c3c8c  9025feeb  bl 0x1084d2d4
108c3c90  0410a0e1  cpy r1, r4
108c3c94  040085e2  add r0, r5, #0x4
108c3c98  7f25feeb  bl 0x1084d29c
108c3c9c  0510a0e1  cpy r1, r5
108c3ca0  0600a0e1  cpy r0, r6
108c3ca4  a78ffeeb  bl 0x10867b48
108c3ca8  0420a0e1  cpy r2, r4
108c3cac  0600a0e1  cpy r0, r6
108c3cb0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c3cb4  0210a0e3  mov r1, #0x2
108c3cb8  2223feea  b 0x1084c948
108c3cbc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
