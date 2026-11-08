108b0194  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b0198  0160a0e1  cpy r6, r1
108b019c  0250a0e1  cpy r5, r2
108b01a0  0200a0e3  mov r0, #0x2
108b01a4  dcedfeeb  bl 0x1086b91c
108b01a8  050050e1  cmp r0, r5
108b01ac  1500001a  bne 0x108b0208
108b01b0  0510a0e1  cpy r1, r5
108b01b4  0600a0e1  cpy r0, r6
108b01b8  b890feeb  bl 0x108544a0
108b01bc  0310a0e3  mov r1, #0x3
108b01c0  e6fdfeeb  bl 0x1086f960
108b01c4  000050e3  cmp r0, #0x0
108b01c8  0e00000a  beq 0x108b0208
108b01cc  0520a0e1  cpy r2, r5
108b01d0  0b10a0e3  mov r1, #0xb
108b01d4  0600a0e1  cpy r0, r6
108b01d8  67e7feeb  bl 0x10869f7c
108b01dc  0040a0e1  cpy r4, r0
108b01e0  1c00a0e3  mov r0, #0x1c
108b01e4  0f50c4e5  strb r5, [r4,#0xf]
108b01e8  b400c4e1  strh r0, [r4,#0x4]
108b01ec  0100a0e3  mov r0, #0x1
108b01f0  116ffeeb  bl 0x1084be3c
108b01f4  080084e5  str r0, [r4,#0x8]
108b01f8  0410a0e1  cpy r1, r4
108b01fc  0600a0e1  cpy r0, r6
108b0200  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b0204  4fdefeea  b 0x10867b48
108b0208  7080bde8  ldmia sp!,{r4,r5,r6,pc}
