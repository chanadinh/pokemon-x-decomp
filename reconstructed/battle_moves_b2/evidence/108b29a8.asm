108b29a8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b29ac  0160a0e1  cpy r6, r1
108b29b0  0250a0e1  cpy r5, r2
108b29b4  0300a0e3  mov r0, #0x3
108b29b8  d7e3feeb  bl 0x1086b91c
108b29bc  050050e1  cmp r0, r5
108b29c0  0e00001a  bne 0x108b2a00
108b29c4  0520a0e1  cpy r2, r5
108b29c8  1f10a0e3  mov r1, #0x1f
108b29cc  0600a0e1  cpy r0, r6
108b29d0  69ddfeeb  bl 0x10869f7c
108b29d4  0040a0e1  cpy r4, r0
108b29d8  0000a0e3  mov r0, #0x0
108b29dc  0400c4e5  strb r0, [r4,#0x4]
108b29e0  0510a0e1  cpy r1, r5
108b29e4  0600a0e1  cpy r0, r6
108b29e8  da01ffeb  bl 0x10873158
108b29ec  0500c4e5  strb r0, [r4,#0x5]
108b29f0  0410a0e1  cpy r1, r4
108b29f4  0600a0e1  cpy r0, r6
108b29f8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b29fc  51d4feea  b 0x10867b48
108b2a00  7080bde8  ldmia sp!,{r4,r5,r6,pc}
