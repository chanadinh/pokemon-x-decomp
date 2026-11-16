108b6e48  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b6e4c  0060a0e1  cpy r6, r0
108b6e50  0240a0e1  cpy r4, r2
108b6e54  0350a0e1  cpy r5, r3
108b6e58  0200a0e3  mov r0, #0x2
108b6e5c  aed2feeb  bl 0x1086b91c
108b6e60  040050e1  cmp r0, r4
108b6e64  00009505  ldreq r0, [r5,#0x0]
108b6e68  02005003  cmpeq r0, #0x2
108b6e6c  0200001a  bne 0x108b6e7c
108b6e70  0600a0e1  cpy r0, r6
108b6e74  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b6e78  d358feea  b 0x1084d1cc
108b6e7c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
