108b53a0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b53a4  0060a0e1  cpy r6, r0
108b53a8  0150a0e1  cpy r5, r1
108b53ac  0240a0e1  cpy r4, r2
108b53b0  0300a0e3  mov r0, #0x3
108b53b4  58d9feeb  bl 0x1086b91c
108b53b8  040050e1  cmp r0, r4
108b53bc  1300001a  bne 0x108b5410
108b53c0  0500a0e1  cpy r0, r5
108b53c4  200080e2  add r0, r0, #0x20
108b53c8  0070a0e1  cpy r7, r0
108b53cc  0600a0e1  cpy r0, r6
108b53d0  b803d0e1  ldrh r0, [r0,#0x38]
108b53d4  0040a0e1  cpy r4, r0
108b53d8  0500a0e1  cpy r0, r5
108b53dc  e05ffeeb  bl 0x1084d364
108b53e0  0020a0e1  cpy r2, r0
108b53e4  0410a0e1  cpy r1, r4
108b53e8  0700a0e1  cpy r0, r7
108b53ec  b55100eb  bl 0x108c9ac8
108b53f0  010050e3  cmp r0, #0x1
108b53f4  0500009a  bls 0x108b5410
108b53f8  3100a0e3  mov r0, #0x31
108b53fc  46d9feeb  bl 0x1086b91c
108b5400  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b5404  8010a0e1  mov r1, r0, lsl #0x1
108b5408  3100a0e3  mov r0, #0x31
108b540c  30c1feea  b 0x108658d4
108b5410  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
