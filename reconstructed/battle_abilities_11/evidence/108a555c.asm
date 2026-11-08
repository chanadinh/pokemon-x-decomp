108a555c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a5560  0150a0e1  cpy r5, r1
108a5564  0070a0e1  cpy r7, r0
108a5568  0260a0e1  cpy r6, r2
108a556c  0380a0e1  cpy r8, r3
108a5570  0200a0e3  mov r0, #0x2
108a5574  e818ffeb  bl 0x1086b91c
108a5578  ff4000e2  and r4, r0, #0xff
108a557c  0610a0e1  cpy r1, r6
108a5580  0500a0e1  cpy r0, r5
108a5584  6f9ffeeb  bl 0x1084d348
108a5588  040050e1  cmp r0, r4
108a558c  1200001a  bne 0x108a55dc
108a5590  1f20a0e3  mov r2, #0x1f
108a5594  2b10a0e3  mov r1, #0x2b
108a5598  0500a0e1  cpy r0, r5
108a559c  7612ffeb  bl 0x10869f7c
108a55a0  0010a0e1  cpy r1, r0
108a55a4  000098e5  ldr r0, [r8,#0x0]
108a55a8  0400c1e5  strb r0, [r1,#0x4]
108a55ac  0500a0e1  cpy r0, r5
108a55b0  0540c1e5  strb r4, [r1,#0x5]
108a55b4  6309ffeb  bl 0x10867b48
108a55b8  0700a0e1  cpy r0, r7
108a55bc  b803d0e1  ldrh r0, [r0,#0x38]
108a55c0  ff1000e2  and r1, r0, #0xff
108a55c4  0620a0e1  cpy r2, r6
108a55c8  0500a0e1  cpy r0, r5
108a55cc  111bffeb  bl 0x1086c218
108a55d0  0700a0e1  cpy r0, r7
108a55d4  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a55d8  fb9efeea  b 0x1084d1cc
108a55dc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
