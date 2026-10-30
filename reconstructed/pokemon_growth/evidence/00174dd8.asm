00174dd8  70402de9  stmdb sp!,{r4,r5,r6,lr}
00174ddc  0040a0e1  cpy r4, r0
00174de0  040090e5  ldr r0, [r0,#0x4]
00174de4  0150a0e1  cpy r5, r1
00174de8  050050e1  cmp r0, r5
00174dec  0400000a  beq 0x00174e04
00174df0  10009fe5  ldr r0, [0x174e08]
00174df4  082084e2  add r2, r4, #0x8
00174df8  000090e5  ldr r0, [r0,#0x0]
00174dfc  040dffeb  bl 0x00138214
00174e00  045084e5  str r5, [r4,#0x4]
00174e04  7080bde8  ldmia sp!,{r4,r5,r6,pc}
