108b47dc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b47e0  0250a0e1  cpy r5, r2
108b47e4  0340a0e1  cpy r4, r3
108b47e8  0300a0e3  mov r0, #0x3
108b47ec  4adcfeeb  bl 0x1086b91c
108b47f0  050050e1  cmp r0, r5
108b47f4  0700001a  bne 0x108b4818
108b47f8  000094e5  ldr r0, [r4,#0x0]
108b47fc  010080e2  add r0, r0, #0x1
108b4800  000084e5  str r0, [r4,#0x0]
108b4804  000180e0  add r0, r0, r0, lsl #0x2
108b4808  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b480c  8010a0e1  mov r1, r0, lsl #0x1
108b4810  3100a0e3  mov r0, #0x31
108b4814  2ec4feea  b 0x108658d4
108b4818  7080bde8  ldmia sp!,{r4,r5,r6,pc}
