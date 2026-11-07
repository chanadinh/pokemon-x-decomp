108b8090  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8094  0150a0e1  cpy r5, r1
108b8098  0240a0e1  cpy r4, r2
108b809c  0300a0e3  mov r0, #0x3
108b80a0  1dcefeeb  bl 0x1086b91c
108b80a4  040050e1  cmp r0, r4
108b80a8  0900001a  bne 0x108b80d4
108b80ac  0500a0e1  cpy r0, r5
108b80b0  684ffeeb  bl 0x1084be58
108b80b4  000050e3  cmp r0, #0x0
108b80b8  0500000a  beq 0x108b80d4
108b80bc  3100a0e3  mov r0, #0x31
108b80c0  15cefeeb  bl 0x1086b91c
108b80c4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b80c8  8010a0e1  mov r1, r0, lsl #0x1
108b80cc  3100a0e3  mov r0, #0x31
108b80d0  ffb5feea  b 0x108658d4
108b80d4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
