108af744  70402de9  stmdb sp!,{r4,r5,r6,lr}
108af748  0160a0e1  cpy r6, r1
108af74c  0250a0e1  cpy r5, r2
108af750  0300a0e3  mov r0, #0x3
108af754  70f0feeb  bl 0x1086b91c
108af758  050050e1  cmp r0, r5
108af75c  1c00001a  bne 0x108af7d4
108af760  0210a0e3  mov r1, #0x2
108af764  0600a0e1  cpy r0, r6
108af768  0173feeb  bl 0x1084c374
108af76c  000050e3  cmp r0, #0x0
108af770  1700001a  bne 0x108af7d4
108af774  0520a0e1  cpy r2, r5
108af778  1c10a0e3  mov r1, #0x1c
108af77c  0600a0e1  cpy r0, r6
108af780  fde9feeb  bl 0x10869f7c
108af784  0040a0e1  cpy r4, r0
108af788  0200a0e3  mov r0, #0x2
108af78c  0400c4e5  strb r0, [r4,#0x4]
108af790  0500a0e3  mov r0, #0x5
108af794  a871feeb  bl 0x1084be3c
108af798  be20d4e1  ldrh r2, [r4,#0xe]
108af79c  080084e5  str r0, [r4,#0x8]
108af7a0  7b10a0e3  mov r1, #0x7b
108af7a4  7f00c2e3  bic r0, r2, #0x7f
108af7a8  bc10c4e1  strh r1, [r4,#0xc]
108af7ac  010080e3  orr r0, r0, #0x1
108af7b0  be00c4e1  strh r0, [r4,#0xe]
108af7b4  0410a0e1  cpy r1, r4
108af7b8  0600a0e1  cpy r0, r6
108af7bc  e1e0feeb  bl 0x10867b48
108af7c0  0520a0e1  cpy r2, r5
108af7c4  0600a0e1  cpy r0, r6
108af7c8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108af7cc  3610a0e3  mov r1, #0x36
108af7d0  5c74feea  b 0x1084c948
108af7d4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
