0017c5d8  70402de9  stmdb sp!,{r4,r5,r6,lr}
0017c5dc  0150a0e1  cpy r5, r1
0017c5e0  081090e5  ldr r1, [r0,#0x8]
0017c5e4  0040a0e1  cpy r4, r0
0017c5e8  080095e5  ldr r0, [r5,#0x8]
0017c5ec  2420a0e3  mov r2, #0x24
0017c5f0  401206eb  bl 0x00300ef8
0017c5f4  b400d4e1  ldrh r0, [r4,#0x4]
0017c5f8  b400c5e1  strh r0, [r5,#0x4]
0017c5fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
