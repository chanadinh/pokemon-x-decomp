108b55d4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b55d8  0150a0e1  cpy r5, r1
108b55dc  0240a0e1  cpy r4, r2
108b55e0  0200a0e3  mov r0, #0x2
108b55e4  ccd8feeb  bl 0x1086b91c
108b55e8  040050e1  cmp r0, r4
108b55ec  0a00001a  bne 0x108b561c
108b55f0  0420a0e1  cpy r2, r4
108b55f4  1710a0e3  mov r1, #0x17
108b55f8  0500a0e1  cpy r0, r5
108b55fc  5ed2feeb  bl 0x10869f7c
108b5600  0010a0e1  cpy r1, r0
108b5604  0800a0e3  mov r0, #0x8
108b5608  0540c1e5  strb r4, [r1,#0x5]
108b560c  0400c1e5  strb r0, [r1,#0x4]
108b5610  0500a0e1  cpy r0, r5
108b5614  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b5618  4ac9feea  b 0x10867b48
108b561c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
