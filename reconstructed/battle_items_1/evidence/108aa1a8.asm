108aa1a8  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108aa1ac  0140a0e1  cpy r4, r1
108aa1b0  0060a0e1  cpy r6, r0
108aa1b4  0250a0e1  cpy r5, r2
108aa1b8  0210a0e1  cpy r1, r2
108aa1bc  0400a0e1  cpy r0, r4
108aa1c0  b6a8feeb  bl 0x108544a0
108aa1c4  0f10a0e3  mov r1, #0xf
108aa1c8  2eb2feeb  bl 0x10856a88
108aa1cc  000050e3  cmp r0, #0x0
108aa1d0  0600001a  bne 0x108aa1f0
108aa1d4  0100a0e3  mov r0, #0x1
108aa1d8  00008de5  str r0, [sp,#0x0]
108aa1dc  0230a0e3  mov r3, #0x2
108aa1e0  0520a0e1  cpy r2, r5
108aa1e4  0410a0e1  cpy r1, r4
108aa1e8  0600a0e1  cpy r0, r6
108aa1ec  e987feeb  bl 0x1084c198
108aa1f0  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
