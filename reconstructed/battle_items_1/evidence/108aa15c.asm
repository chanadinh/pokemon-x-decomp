108aa15c  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108aa160  0140a0e1  cpy r4, r1
108aa164  0060a0e1  cpy r6, r0
108aa168  0250a0e1  cpy r5, r2
108aa16c  0210a0e1  cpy r1, r2
108aa170  0400a0e1  cpy r0, r4
108aa174  c9a8feeb  bl 0x108544a0
108aa178  0f10a0e3  mov r1, #0xf
108aa17c  41b2feeb  bl 0x10856a88
108aa180  000050e3  cmp r0, #0x0
108aa184  0600001a  bne 0x108aa1a4
108aa188  0000a0e3  mov r0, #0x0
108aa18c  00008de5  str r0, [sp,#0x0]
108aa190  0230a0e3  mov r3, #0x2
108aa194  0520a0e1  cpy r2, r5
108aa198  0410a0e1  cpy r1, r4
108aa19c  0600a0e1  cpy r0, r6
108aa1a0  fc87feeb  bl 0x1084c198
108aa1a4  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
