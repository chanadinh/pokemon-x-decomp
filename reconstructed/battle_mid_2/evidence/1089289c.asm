1089289c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108928a0  0040a0e1  cpy r4, r0
108928a4  0000a0e3  mov r0, #0x0
108928a8  2400c4e5  strb r0, [r4,#0x24]
108928ac  0300a0e3  mov r0, #0x3
108928b0  0060a0e1  cpy r6, r0
108928b4  d900c4e5  strb r0, [r4,#0xd9]
108928b8  ec0094e5  ldr r0, [r4,#0xec]
108928bc  0050a0e3  mov r5, #0x0
108928c0  2bbbfdeb  bl 0x10801574
108928c4  f450c4e5  strb r5, [r4,#0xf4]
108928c8  4110a0e3  mov r1, #0x41
108928cc  f560c4e5  strb r6, [r4,#0xf5]
108928d0  0400a0e1  cpy r0, r4
108928d4  f610c4e5  strb r1, [r4,#0xf6]
108928d8  6bfbfeeb  bl 0x1085168c
108928dc  0200a0e3  mov r0, #0x2
108928e0  0060a0e1  cpy r6, r0
108928e4  d900c4e5  strb r0, [r4,#0xd9]
108928e8  ec0094e5  ldr r0, [r4,#0xec]
108928ec  0150a0e3  mov r5, #0x1
108928f0  21bbfdeb  bl 0x1080157c
108928f4  f450c4e5  strb r5, [r4,#0xf4]
108928f8  f560c4e5  strb r6, [r4,#0xf5]
108928fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
