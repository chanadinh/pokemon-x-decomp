108aa8e8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aa8ec  0160a0e1  cpy r6, r1
108aa8f0  0250a0e1  cpy r5, r2
108aa8f4  0200a0e3  mov r0, #0x2
108aa8f8  0704ffeb  bl 0x1086b91c
108aa8fc  050050e1  cmp r0, r5
108aa900  0f00001a  bne 0x108aa944
108aa904  0300a0e3  mov r0, #0x3
108aa908  0304ffeb  bl 0x1086b91c
108aa90c  ff2000e2  and r2, r0, #0xff
108aa910  0b10a0e3  mov r1, #0xb
108aa914  0600a0e1  cpy r0, r6
108aa918  97fdfeeb  bl 0x10869f7c
108aa91c  0040a0e1  cpy r4, r0
108aa920  0500a0e3  mov r0, #0x5
108aa924  0f50c4e5  strb r5, [r4,#0xf]
108aa928  b400c4e1  strh r0, [r4,#0x4]
108aa92c  7900a0e3  mov r0, #0x79
108aa930  080084e5  str r0, [r4,#0x8]
108aa934  0410a0e1  cpy r1, r4
108aa938  0600a0e1  cpy r0, r6
108aa93c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aa940  80f4feea  b 0x10867b48
108aa944  7080bde8  ldmia sp!,{r4,r5,r6,pc}
