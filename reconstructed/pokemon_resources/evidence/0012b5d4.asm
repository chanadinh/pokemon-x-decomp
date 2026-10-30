0012b5d4  70402de9  stmdb sp!,{r4,r5,r6,lr}
0012b5d8  0040a0e1  cpy r4, r0
0012b5dc  0000a0e3  mov r0, #0x0
0012b5e0  b000c4e1  strh r0, [r4,#0x0]
0012b5e4  0200c4e5  strb r0, [r4,#0x2]
0012b5e8  040084e5  str r0, [r4,#0x4]
0012b5ec  0800c4e5  strb r0, [r4,#0x8]
0012b5f0  6800a0e3  mov r0, #0x68
0012b5f4  c9dcffeb  bl 0x00122920
0012b5f8  0050b0e1  movs r5, r0
0012b5fc  6810a013  movne r1, #0x68
0012b600  c3cc001b  blne 0x0015e914
0012b604  0400a0e1  cpy r0, r4
0012b608  045084e5  str r5, [r4,#0x4]
0012b60c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
