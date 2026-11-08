108b39e8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b39ec  0150a0e1  cpy r5, r1
108b39f0  0240a0e1  cpy r4, r2
108b39f4  0200a0e3  mov r0, #0x2
108b39f8  c7dffeeb  bl 0x1086b91c
108b39fc  040050e1  cmp r0, r4
108b3a00  0a00001a  bne 0x108b3a30
108b3a04  0420a0e1  cpy r2, r4
108b3a08  1710a0e3  mov r1, #0x17
108b3a0c  0500a0e1  cpy r0, r5
108b3a10  59d9feeb  bl 0x10869f7c
108b3a14  0010a0e1  cpy r1, r0
108b3a18  0700a0e3  mov r0, #0x7
108b3a1c  0540c1e5  strb r4, [r1,#0x5]
108b3a20  0400c1e5  strb r0, [r1,#0x4]
108b3a24  0500a0e1  cpy r0, r5
108b3a28  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b3a2c  45d0feea  b 0x10867b48
108b3a30  7080bde8  ldmia sp!,{r4,r5,r6,pc}
