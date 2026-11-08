108a90a0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a90a4  0160a0e1  cpy r6, r1
108a90a8  0250a0e1  cpy r5, r2
108a90ac  0200a0e3  mov r0, #0x2
108a90b0  190affeb  bl 0x1086b91c
108a90b4  050050e1  cmp r0, r5
108a90b8  1000001a  bne 0x108a9100
108a90bc  0300a0e3  mov r0, #0x3
108a90c0  150affeb  bl 0x1086b91c
108a90c4  ff2000e2  and r2, r0, #0xff
108a90c8  0b10a0e3  mov r1, #0xb
108a90cc  0600a0e1  cpy r0, r6
108a90d0  a903ffeb  bl 0x10869f7c
108a90d4  0410a0e3  mov r1, #0x4
108a90d8  0f50c0e5  strb r5, [r0,#0xf]
108a90dc  0040a0e1  cpy r4, r0
108a90e0  b410c0e1  strh r1, [r0,#0x4]
108a90e4  0100a0e1  cpy r0, r1
108a90e8  44dffeeb  bl 0x10860e00
108a90ec  080084e5  str r0, [r4,#0x8]
108a90f0  0410a0e1  cpy r1, r4
108a90f4  0600a0e1  cpy r0, r6
108a90f8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a90fc  91fafeea  b 0x10867b48
108a9100  7080bde8  ldmia sp!,{r4,r5,r6,pc}
