108c4fa4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c4fa8  0340a0e1  cpy r4, r3
108c4fac  0170a0e1  cpy r7, r1
108c4fb0  80809fe5  ldr r8, [0x108c5038]
108c4fb4  0250a0e1  cpy r5, r2
108c4fb8  0400a0e3  mov r0, #0x4
108c4fbc  569afeeb  bl 0x1086b91c
108c4fc0  050050e1  cmp r0, r5
108c4fc4  00009405  ldreq r0, [r4,#0x0]
108c4fc8  01005003  cmpeq r0, #0x1
108c4fcc  1800001a  bne 0x108c5034
108c4fd0  0520a0e1  cpy r2, r5
108c4fd4  0110a0e3  mov r1, #0x1
108c4fd8  0700a0e1  cpy r0, r7
108c4fdc  591efeeb  bl 0x1084c948
108c4fe0  0520a0e1  cpy r2, r5
108c4fe4  0310a0e3  mov r1, #0x3
108c4fe8  0700a0e1  cpy r0, r7
108c4fec  e293feeb  bl 0x10869f7c
108c4ff0  0060a0e1  cpy r6, r0
108c4ff4  0820a0e1  cpy r2, r8
108c4ff8  0210a0e3  mov r1, #0x2
108c4ffc  040080e2  add r0, r0, #0x4
108c5000  b320feeb  bl 0x1084d2d4
108c5004  0510a0e1  cpy r1, r5
108c5008  040086e2  add r0, r6, #0x4
108c500c  a220feeb  bl 0x1084d29c
108c5010  0610a0e1  cpy r1, r6
108c5014  0700a0e1  cpy r0, r7
108c5018  ca8afeeb  bl 0x10867b48
108c501c  0520a0e1  cpy r2, r5
108c5020  0210a0e3  mov r1, #0x2
108c5024  0700a0e1  cpy r0, r7
108c5028  461efeeb  bl 0x1084c948
108c502c  0000a0e3  mov r0, #0x0
108c5030  000084e5  str r0, [r4,#0x0]
108c5034  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
