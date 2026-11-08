108bb5e8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb5ec  0160a0e1  cpy r6, r1
108bb5f0  0240a0e1  cpy r4, r2
108bb5f4  0300a0e3  mov r0, #0x3
108bb5f8  c7c0feeb  bl 0x1086b91c
108bb5fc  040050e1  cmp r0, r4
108bb600  1f00001a  bne 0x108bb684
108bb604  0420a0e1  cpy r2, r4
108bb608  0310a0e3  mov r1, #0x3
108bb60c  0600a0e1  cpy r0, r6
108bb610  59bafeeb  bl 0x10869f7c
108bb614  0050a0e1  cpy r5, r0
108bb618  8b2fa0e3  mov r2, #0x22c
108bb61c  0210a0e3  mov r1, #0x2
108bb620  040080e2  add r0, r0, #0x4
108bb624  2a47feeb  bl 0x1084d2d4
108bb628  0410a0e1  cpy r1, r4
108bb62c  040085e2  add r0, r5, #0x4
108bb630  1947feeb  bl 0x1084d29c
108bb634  0510a0e1  cpy r1, r5
108bb638  0600a0e1  cpy r0, r6
108bb63c  41b1feeb  bl 0x10867b48
108bb640  0420a0e1  cpy r2, r4
108bb644  0d10a0e3  mov r1, #0xd
108bb648  0600a0e1  cpy r0, r6
108bb64c  4abafeeb  bl 0x10869f7c
108bb650  1820d0e5  ldrb r2, [r0,#0x18]
108bb654  0010a0e1  cpy r1, r0
108bb658  0130a0e3  mov r3, #0x1
108bb65c  0c40c1e5  strb r4, [r1,#0xc]
108bb660  0200a0e3  mov r0, #0x2
108bb664  1630c1e5  strb r3, [r1,#0x16]
108bb668  0400c1e5  strb r0, [r1,#0x4]
108bb66c  062082e3  orr r2, r2, #0x6
108bb670  1730c1e5  strb r3, [r1,#0x17]
108bb674  0600a0e1  cpy r0, r6
108bb678  1820c1e5  strb r2, [r1,#0x18]
108bb67c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb680  30b1feea  b 0x10867b48
108bb684  7080bde8  ldmia sp!,{r4,r5,r6,pc}
