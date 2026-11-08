108a8ddc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a8de0  0160a0e1  cpy r6, r1
108a8de4  0250a0e1  cpy r5, r2
108a8de8  0200a0e3  mov r0, #0x2
108a8dec  ca0affeb  bl 0x1086b91c
108a8df0  050050e1  cmp r0, r5
108a8df4  0f00001a  bne 0x108a8e38
108a8df8  0300a0e3  mov r0, #0x3
108a8dfc  c60affeb  bl 0x1086b91c
108a8e00  ff2000e2  and r2, r0, #0xff
108a8e04  0b10a0e3  mov r1, #0xb
108a8e08  0600a0e1  cpy r0, r6
108a8e0c  5a04ffeb  bl 0x10869f7c
108a8e10  0040a0e1  cpy r4, r0
108a8e14  0500a0e3  mov r0, #0x5
108a8e18  0f50c4e5  strb r5, [r4,#0xf]
108a8e1c  b400c4e1  strh r0, [r4,#0x4]
108a8e20  f6dffeeb  bl 0x10860e00
108a8e24  080084e5  str r0, [r4,#0x8]
108a8e28  0410a0e1  cpy r1, r4
108a8e2c  0600a0e1  cpy r0, r6
108a8e30  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a8e34  43fbfeea  b 0x10867b48
108a8e38  7080bde8  ldmia sp!,{r4,r5,r6,pc}
