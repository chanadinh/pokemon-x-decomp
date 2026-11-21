10893568  70402de9  stmdb sp!,{r4,r5,r6,lr}
1089356c  0040a0e1  cpy r4, r0
10893570  0000a0e3  mov r0, #0x0
10893574  2400c4e5  strb r0, [r4,#0x24]
10893578  0300a0e3  mov r0, #0x3
1089357c  0060a0e1  cpy r6, r0
10893580  d900c4e5  strb r0, [r4,#0xd9]
10893584  ec0094e5  ldr r0, [r4,#0xec]
10893588  0050a0e3  mov r5, #0x0
1089358c  f8b7fdeb  bl 0x10801574
10893590  f450c4e5  strb r5, [r4,#0xf4]
10893594  4110a0e3  mov r1, #0x41
10893598  f560c4e5  strb r6, [r4,#0xf5]
1089359c  0400a0e1  cpy r0, r4
108935a0  f610c4e5  strb r1, [r4,#0xf6]
108935a4  07f7feeb  bl 0x108511c8
108935a8  ec0094e5  ldr r0, [r4,#0xec]
108935ac  0150a0e3  mov r5, #0x1
108935b0  0260a0e3  mov r6, #0x2
108935b4  f0b7fdeb  bl 0x1080157c
108935b8  f450c4e5  strb r5, [r4,#0xf4]
108935bc  0200a0e3  mov r0, #0x2
108935c0  f560c4e5  strb r6, [r4,#0xf5]
108935c4  d900c4e5  strb r0, [r4,#0xd9]
108935c8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
