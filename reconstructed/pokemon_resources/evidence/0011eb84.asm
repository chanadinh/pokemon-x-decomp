0011eb84  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0011eb88  0050a0e1  cpy r5, r0
0011eb8c  0170a0e1  cpy r7, r1
0011eb90  64109fe5  ldr r1, [0x11ebfc]
0011eb94  0000a0e3  mov r0, #0x0
0011eb98  040085e5  str r0, [r5,#0x4]
0011eb9c  001085e5  str r1, [r5,#0x0]
0011eba0  0c0085e5  str r0, [r5,#0xc]
0011eba4  0260a0e1  cpy r6, r2
0011eba8  0201a0e1  mov r0, r2, lsl #0x2
0011ebac  0710a0e1  cpy r1, r7
0011ebb0  082085e5  str r2, [r5,#0x8]
0011ebb4  4c0f00eb  bl 0x001228ec
0011ebb8  000056e3  cmp r6, #0x0
0011ebbc  0040a0e3  mov r4, #0x0
0011ebc0  040085e5  str r0, [r5,#0x4]
0011ebc4  0a00009a  bls 0x0011ebf4
0011ebc8  0710a0e1  cpy r1, r7
0011ebcc  0c00a0e3  mov r0, #0xc
0011ebd0  520f00eb  bl 0x00122920
0011ebd4  000050e3  cmp r0, #0x0
0011ebd8  0710a011  cpyne r1, r7
0011ebdc  7c32001b  blne 0x0012b5d4
0011ebe0  041095e5  ldr r1, [r5,#0x4]
0011ebe4  040181e7  str r0, [r1,r4,lsl #0x2]
0011ebe8  014084e2  add r4, r4, #0x1
0011ebec  060054e1  cmp r4, r6
0011ebf0  f4ffff3a  bcc 0x0011ebc8
0011ebf4  0500a0e1  cpy r0, r5
0011ebf8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
