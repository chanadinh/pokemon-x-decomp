108b45bc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b45c0  0050a0e3  mov r5, #0x0
108b45c4  0180a0e1  cpy r8, r1
108b45c8  0270a0e1  cpy r7, r2
108b45cc  0300a0e3  mov r0, #0x3
108b45d0  d1dcfeeb  bl 0x1086b91c
108b45d4  070050e1  cmp r0, r7
108b45d8  2500001a  bne 0x108b4674
108b45dc  0500a0e3  mov r0, #0x5
108b45e0  cddcfeeb  bl 0x1086b91c
108b45e4  006050e2  subs r6, r0, #0x0
108b45e8  0040a0e3  mov r4, #0x0
108b45ec  2000009a  bls 0x108b4674
108b45f0  060084e2  add r0, r4, #0x6
108b45f4  ff0000e2  and r0, r0, #0xff
108b45f8  c7dcfeeb  bl 0x1086b91c
108b45fc  ff1000e2  and r1, r0, #0xff
108b4600  0800a0e1  cpy r0, r8
108b4604  a57ffeeb  bl 0x108544a0
108b4608  00f020e3  nop
108b460c  00f020e3  nop
108b4610  8bacfeeb  bl 0x1085f844
108b4614  000050e3  cmp r0, #0x0
108b4618  01508512  addne r5, r5, #0x1
108b461c  014084e2  add r4, r4, #0x1
108b4620  040056e1  cmp r6, r4
108b4624  f1ffff8a  bhi 0x108b45f0
108b4628  000055e3  cmp r5, #0x0
108b462c  1000000a  beq 0x108b4674
108b4630  0720a0e1  cpy r2, r7
108b4634  0d10a0e3  mov r1, #0xd
108b4638  0800a0e1  cpy r0, r8
108b463c  4ed6feeb  bl 0x10869f7c
108b4640  1830d0e5  ldrb r3, [r0,#0x18]
108b4644  0010a0e1  cpy r1, r0
108b4648  0120a0e3  mov r2, #0x1
108b464c  0c70c1e5  strb r7, [r1,#0xc]
108b4650  1620c1e5  strb r2, [r1,#0x16]
108b4654  0200a0e3  mov r0, #0x2
108b4658  0420c1e5  strb r2, [r1,#0x4]
108b465c  1700c1e5  strb r0, [r1,#0x17]
108b4660  043083e3  orr r3, r3, #0x4
108b4664  0800a0e1  cpy r0, r8
108b4668  1830c1e5  strb r3, [r1,#0x18]
108b466c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b4670  34cdfeea  b 0x10867b48
108b4674  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
