0011ec7c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0011ec80  0050a0e1  cpy r5, r0
0011ec84  0170a0e1  cpy r7, r1
0011ec88  64109fe5  ldr r1, [0x11ecf4]
0011ec8c  0000a0e3  mov r0, #0x0
0011ec90  040085e5  str r0, [r5,#0x4]
0011ec94  001085e5  str r1, [r5,#0x0]
0011ec98  0c0085e5  str r0, [r5,#0xc]
0011ec9c  0260a0e1  cpy r6, r2
0011eca0  0201a0e1  mov r0, r2, lsl #0x2
0011eca4  0710a0e1  cpy r1, r7
0011eca8  082085e5  str r2, [r5,#0x8]
0011ecac  0e0f00eb  bl 0x001228ec
0011ecb0  000056e3  cmp r6, #0x0
0011ecb4  0040a0e3  mov r4, #0x0
0011ecb8  040085e5  str r0, [r5,#0x4]
0011ecbc  0a00009a  bls 0x0011ecec
0011ecc0  0710a0e1  cpy r1, r7
0011ecc4  0c00a0e3  mov r0, #0xc
0011ecc8  140f00eb  bl 0x00122920
0011eccc  000050e3  cmp r0, #0x0
0011ecd0  0710a011  cpyne r1, r7
0011ecd4  5d32001b  blne 0x0012b650
0011ecd8  041095e5  ldr r1, [r5,#0x4]
0011ecdc  040181e7  str r0, [r1,r4,lsl #0x2]
0011ece0  014084e2  add r4, r4, #0x1
0011ece4  060054e1  cmp r4, r6
0011ece8  f4ffff3a  bcc 0x0011ecc0
0011ecec  0500a0e1  cpy r0, r5
0011ecf0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
