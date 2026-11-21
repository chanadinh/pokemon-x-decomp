1083b110  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1083b114  0040a0e1  cpy r4, r0
1083b118  0170a0e1  cpy r7, r1
1083b11c  4c109fe5  ldr r1, [0x1083b170]
1083b120  0250a0e1  cpy r5, r2
1083b124  48009fe5  ldr r0, [0x1083b174]
1083b128  6c2091e5  ldr r2, [r1,#0x6c]
1083b12c  000052e1  cmp r2, r0
1083b130  6c008115  strne r0, [r1,#0x6c]
1083b134  060c84e2  add r0, r4, #0x600
1083b138  ca0080e2  add r0, r0, #0xca
1083b13c  ff10a0e3  mov r1, #0xff
1083b140  0060a0e1  cpy r6, r0
1083b144  f3d200eb  bl 0x1086fd18
1083b148  0020a0e3  mov r2, #0x0
1083b14c  5d22c4e5  strb r2, [r4,#0x25d]
1083b150  c876c4e5  strb r7, [r4,#0x6c8]
1083b154  c956c4e5  strb r5, [r4,#0x6c9]
1083b158  0710a0e1  cpy r1, r7
1083b15c  0400a0e1  cpy r0, r4
1083b160  0050c6e5  strb r5, [r6,#0x0]
1083b164  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1083b168  08209fe5  ldr r2, [0x1083b178]
1083b16c  be7000ea  b 0x1085746c
