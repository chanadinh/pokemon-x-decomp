1089498c  70402de9  stmdb sp!,{r4,r5,r6,lr}
10894990  0040a0e1  cpy r4, r0
10894994  0000a0e3  mov r0, #0x0
10894998  2400c4e5  strb r0, [r4,#0x24]
1089499c  0300a0e3  mov r0, #0x3
108949a0  0060a0e1  cpy r6, r0
108949a4  d900c4e5  strb r0, [r4,#0xd9]
108949a8  ec0094e5  ldr r0, [r4,#0xec]
108949ac  0050a0e3  mov r5, #0x0
108949b0  efb2fdeb  bl 0x10801574
108949b4  f450c4e5  strb r5, [r4,#0xf4]
108949b8  5f10a0e3  mov r1, #0x5f
108949bc  f560c4e5  strb r6, [r4,#0xf5]
108949c0  0400a0e1  cpy r0, r4
108949c4  f610c4e5  strb r1, [r4,#0xf6]
108949c8  c9f0feeb  bl 0x10850cf4
108949cc  ec0094e5  ldr r0, [r4,#0xec]
108949d0  0150a0e3  mov r5, #0x1
108949d4  0260a0e3  mov r6, #0x2
108949d8  e7b2fdeb  bl 0x1080157c
108949dc  f450c4e5  strb r5, [r4,#0xf4]
108949e0  0200a0e3  mov r0, #0x2
108949e4  f560c4e5  strb r6, [r4,#0xf5]
108949e8  d900c4e5  strb r0, [r4,#0xd9]
108949ec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
