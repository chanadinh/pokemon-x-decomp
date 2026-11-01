108a6698  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a669c  0060a0e1  cpy r6, r0
108a66a0  0150a0e1  cpy r5, r1
108a66a4  0240a0e1  cpy r4, r2
108a66a8  0300a0e3  mov r0, #0x3
108a66ac  9a14ffeb  bl 0x1086b91c
108a66b0  040050e1  cmp r0, r4
108a66b4  3000001a  bne 0x108a677c
108a66b8  1200a0e3  mov r0, #0x12
108a66bc  9614ffeb  bl 0x1086b91c
108a66c0  7000ffe6  uxth r0, r0
108a66c4  4f98feeb  bl 0x1084c808
108a66c8  000050e3  cmp r0, #0x0
108a66cc  2a00001a  bne 0x108a677c
108a66d0  0410a0e1  cpy r1, r4
108a66d4  0500a0e1  cpy r0, r5
108a66d8  70b7feeb  bl 0x108544a0
108a66dc  0070a0e1  cpy r7, r0
108a66e0  f88700eb  bl 0x108c86c8
108a66e4  0048a0e1  mov r4, r0, lsl #0x10
108a66e8  2448b0e1  movs r4, r4, lsr #0x10
108a66ec  2200000a  beq 0x108a677c
108a66f0  1200a0e3  mov r0, #0x12
108a66f4  8814ffeb  bl 0x1086b91c
108a66f8  7050ffe6  uxth r5, r0
108a66fc  0700a0e1  cpy r0, r7
108a6700  3d98feeb  bl 0x1084c7fc
108a6704  000055e1  cmp r5, r0
108a6708  1b00001a  bne 0x108a677c
108a670c  0600a0e1  cpy r0, r6
108a6710  0350a0e3  mov r5, #0x3
108a6714  b803d0e1  ldrh r0, [r0,#0x38]
108a6718  0510a0e1  cpy r1, r5
108a671c  149afeeb  bl 0x1084cf74
108a6720  900400e0  mul r0, r0, r4
108a6724  150a9fed  vldr.32 s0, [pc,#0x54]
108a6728  640080e2  add r0, r0, #0x64
108a672c  7000ffe6  uxth r0, r0
108a6730  c80050e3  cmp r0, #0xc8
108a6734  c800a083  movhi r0, #0xc8
108a6738  0200008a  bhi 0x108a6748
108a673c  000050e3  cmp r0, #0x0
108a6740  0f0a9f0d  vldreq.32 s0, [pc,#0x3c]
108a6744  0300000a  beq 0x108a6758
108a6748  0006a0e1  mov r0, r0, lsl #0xc
108a674c  900a00ee  vmov s1, r0
108a6750  600af8ee  vcvt.f32.u32 s1, s1
108a6754  800a30ee  vadd.f32 s0, s1, s0
108a6758  28109fe5  ldr r1, [0x108a6788]
108a675c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a6760  c00abdee  vcvt.s32.f32 s0, s0
108a6764  100a10ee  vmov r0, s0
108a6768  9110c0e0  smull r1, r0, r1, r0
108a676c  c012a0e1  mov r1, r0, asr #0x5
108a6770  c01f41e0  sub r1, r1, r0, asr #0x1f
108a6774  3600a0e3  mov r0, #0x36
108a6778  c199feea  b 0x1084ce84
108a677c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
