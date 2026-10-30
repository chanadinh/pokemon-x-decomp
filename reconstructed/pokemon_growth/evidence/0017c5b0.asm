0017c5b0  70402de9  stmdb sp!,{r4,r5,r6,lr}
0017c5b4  0150a0e1  cpy r5, r1
0017c5b8  0040a0e1  cpy r4, r0
0017c5bc  081080e2  add r1, r0, #0x8
0017c5c0  652fa0e3  mov r2, #0x194
0017c5c4  080085e2  add r0, r5, #0x8
0017c5c8  5922ffeb  bl 0x00144f34
0017c5cc  040094e5  ldr r0, [r4,#0x4]
0017c5d0  040085e5  str r0, [r5,#0x4]
0017c5d4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
