108ba4a0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba4a4  0150a0e1  cpy r5, r1
108ba4a8  0240a0e1  cpy r4, r2
108ba4ac  0200a0e3  mov r0, #0x2
108ba4b0  19c5feeb  bl 0x1086b91c
108ba4b4  040050e1  cmp r0, r4
108ba4b8  2000001a  bne 0x108ba540
108ba4bc  0410a0e1  cpy r1, r4
108ba4c0  0500a0e1  cpy r0, r5
108ba4c4  f567feeb  bl 0x108544a0
108ba4c8  b201d0e1  ldrh r0, [r0,#0x12]
108ba4cc  000050e3  cmp r0, #0x0
108ba4d0  1600000a  beq 0x108ba530
108ba4d4  0610a0e3  mov r1, #0x6
108ba4d8  a54afeeb  bl 0x1084cf74
108ba4dc  000050e3  cmp r0, #0x0
108ba4e0  1200000a  beq 0x108ba530
108ba4e4  0410a0e1  cpy r1, r4
108ba4e8  0500a0e1  cpy r0, r5
108ba4ec  3144feeb  bl 0x1084b5b8
108ba4f0  000050e3  cmp r0, #0x0
108ba4f4  0d00000a  beq 0x108ba530
108ba4f8  0410a0e1  cpy r1, r4
108ba4fc  0500a0e1  cpy r0, r5
108ba500  e667feeb  bl 0x108544a0
108ba504  0040a0e1  cpy r4, r0
108ba508  bc00d0e1  ldrh r0, [r0,#0xc]
108ba50c  0060a0e1  cpy r6, r0
108ba510  0400a0e1  cpy r0, r4
108ba514  b201d0e1  ldrh r0, [r0,#0x12]
108ba518  0020a0e1  cpy r2, r0
108ba51c  0610a0e1  cpy r1, r6
108ba520  0500a0e1  cpy r0, r5
108ba524  bc67feeb  bl 0x1085441c
108ba528  000050e3  cmp r0, #0x0
108ba52c  0300000a  beq 0x108ba540
108ba530  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba534  1b10a0e3  mov r1, #0x1b
108ba538  2300a0e3  mov r0, #0x23
108ba53c  e4acfeea  b 0x108658d4
108ba540  7080bde8  ldmia sp!,{r4,r5,r6,pc}
