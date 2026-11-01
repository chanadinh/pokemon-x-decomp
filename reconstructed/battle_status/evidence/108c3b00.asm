108c3b00  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108c3b04  0340a0e1  cpy r4, r3
108c3b08  0160a0e1  cpy r6, r1
108c3b0c  0270a0e1  cpy r7, r2
108c3b10  8c909fe5  ldr r9, [0x108c3ba4]
108c3b14  0400a0e3  mov r0, #0x4
108c3b18  7f9ffeeb  bl 0x1086b91c
108c3b1c  ff8000e2  and r8, r0, #0xff
108c3b20  0810a0e1  cpy r1, r8
108c3b24  0700a0e1  cpy r0, r7
108c3b28  9e22feeb  bl 0x1084c5a8
108c3b2c  000050e3  cmp r0, #0x0
108c3b30  00009415  ldrne r0, [r4,#0x0]
108c3b34  00005013  cmpne r0, #0x0
108c3b38  1800000a  beq 0x108c3ba0
108c3b3c  0720a0e1  cpy r2, r7
108c3b40  0110a0e3  mov r1, #0x1
108c3b44  0600a0e1  cpy r0, r6
108c3b48  7e23feeb  bl 0x1084c948
108c3b4c  0720a0e1  cpy r2, r7
108c3b50  0310a0e3  mov r1, #0x3
108c3b54  0600a0e1  cpy r0, r6
108c3b58  0799feeb  bl 0x10869f7c
108c3b5c  0050a0e1  cpy r5, r0
108c3b60  0920a0e1  cpy r2, r9
108c3b64  0210a0e3  mov r1, #0x2
108c3b68  040080e2  add r0, r0, #0x4
108c3b6c  d825feeb  bl 0x1084d2d4
108c3b70  0810a0e1  cpy r1, r8
108c3b74  040085e2  add r0, r5, #0x4
108c3b78  c725feeb  bl 0x1084d29c
108c3b7c  0510a0e1  cpy r1, r5
108c3b80  0600a0e1  cpy r0, r6
108c3b84  ef8ffeeb  bl 0x10867b48
108c3b88  0720a0e1  cpy r2, r7
108c3b8c  0210a0e3  mov r1, #0x2
108c3b90  0600a0e1  cpy r0, r6
108c3b94  6b23feeb  bl 0x1084c948
108c3b98  0000a0e3  mov r0, #0x0
108c3b9c  000084e5  str r0, [r4,#0x0]
108c3ba0  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
