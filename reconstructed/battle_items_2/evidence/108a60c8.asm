108a60c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a60cc  0160a0e1  cpy r6, r1
108a60d0  0240a0e1  cpy r4, r2
108a60d4  0450a0e3  mov r5, #0x4
108a60d8  0200a0e3  mov r0, #0x2
108a60dc  0e16ffeb  bl 0x1086b91c
108a60e0  040050e1  cmp r0, r4
108a60e4  0c00001a  bne 0x108a611c
108a60e8  0420a0e1  cpy r2, r4
108a60ec  0d10a0e3  mov r1, #0xd
108a60f0  0600a0e1  cpy r0, r6
108a60f4  a00fffeb  bl 0x10869f7c
108a60f8  0120a0e3  mov r2, #0x1
108a60fc  0450c0e5  strb r5, [r0,#0x4]
108a6100  1720c0e5  strb r2, [r0,#0x17]
108a6104  0c40c0e5  strb r4, [r0,#0xc]
108a6108  0010a0e1  cpy r1, r0
108a610c  1620c0e5  strb r2, [r0,#0x16]
108a6110  0600a0e1  cpy r0, r6
108a6114  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6118  8a06ffea  b 0x10867b48
108a611c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
