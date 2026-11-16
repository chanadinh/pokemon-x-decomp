108b7b50  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b7b54  0060a0e1  cpy r6, r0
108b7b58  0240a0e1  cpy r4, r2
108b7b5c  0350a0e1  cpy r5, r3
108b7b60  0300a0e3  mov r0, #0x3
108b7b64  6ccffeeb  bl 0x1086b91c
108b7b68  040050e1  cmp r0, r4
108b7b6c  0700001a  bne 0x108b7b90
108b7b70  1200a0e3  mov r0, #0x12
108b7b74  68cffeeb  bl 0x1086b91c
108b7b78  0040a0e1  cpy r4, r0
108b7b7c  0600a0e1  cpy r0, r6
108b7b80  b803d0e1  ldrh r0, [r0,#0x38]
108b7b84  000054e1  cmp r4, r0
108b7b88  0200a013  movne r0, #0x2
108b7b8c  00008515  strne r0, [r5,#0x0]
108b7b90  7080bde8  ldmia sp!,{r4,r5,r6,pc}
