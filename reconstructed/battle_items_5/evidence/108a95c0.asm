108a95c0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a95c4  0160a0e1  cpy r6, r1
108a95c8  0250a0e1  cpy r5, r2
108a95cc  0200a0e3  mov r0, #0x2
108a95d0  d108ffeb  bl 0x1086b91c
108a95d4  050050e1  cmp r0, r5
108a95d8  0f00001a  bne 0x108a961c
108a95dc  0300a0e3  mov r0, #0x3
108a95e0  cd08ffeb  bl 0x1086b91c
108a95e4  ff2000e2  and r2, r0, #0xff
108a95e8  0b10a0e3  mov r1, #0xb
108a95ec  0600a0e1  cpy r0, r6
108a95f0  6102ffeb  bl 0x10869f7c
108a95f4  0040a0e1  cpy r4, r0
108a95f8  0100a0e3  mov r0, #0x1
108a95fc  0f50c4e5  strb r5, [r4,#0xf]
108a9600  b400c4e1  strh r0, [r4,#0x4]
108a9604  fdddfeeb  bl 0x10860e00
108a9608  080084e5  str r0, [r4,#0x8]
108a960c  0410a0e1  cpy r1, r4
108a9610  0600a0e1  cpy r0, r6
108a9614  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a9618  4af9feea  b 0x10867b48
108a961c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
