1083f3ac  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1083f3b0  0050a0e1  cpy r5, r0
1083f3b4  017b85e2  add r7, r5, #0x400
1083f3b8  7c109fe5  ldr r1, [0x1083f43c]
1083f3bc  7c009fe5  ldr r0, [0x1083f440]
1083f3c0  0160a0e3  mov r6, #0x1
1083f3c4  0040a0e3  mov r4, #0x0
1083f3c8  6c2091e5  ldr r2, [r1,#0x6c]
1083f3cc  ad7f87e2  add r7, r7, #0x2b4
1083f3d0  000052e1  cmp r2, r0
1083f3d4  6c008115  strne r0, [r1,#0x6c]
1083f3d8  040085e0  add r0, r5, r4
1083f3dc  ca16d0e5  ldrb r1, [r0,#0x6ca]
1083f3e0  ff0051e3  cmp r1, #0xff
1083f3e4  0700000a  beq 0x1083f408
1083f3e8  000064e2  rsb r0, r4, #0x0
1083f3ec  b42695e5  ldr r2, [r5,#0x6b4]
1083f3f0  000180e0  add r0, r0, r0, lsl #0x2
1083f3f4  802082e0  add r2, r2, r0, lsl #0x1
1083f3f8  0500a0e1  cpy r0, r5
1083f3fc  83f2ffeb  bl 0x1083be10
1083f400  000050e3  cmp r0, #0x0
1083f404  0060a003  moveq r6, #0x0
1083f408  014084e2  add r4, r4, #0x1
1083f40c  060054e3  cmp r4, #0x6
1083f410  f0ffffba  blt 0x1083f3d8
1083f414  b40695e5  ldr r0, [r5,#0x6b4]
1083f418  000056e3  cmp r6, #0x0
1083f41c  010080e2  add r0, r0, #0x1
1083f420  000087e5  str r0, [r7,#0x0]
1083f424  0200000a  beq 0x1083f434
1083f428  900195e5  ldr r0, [r5,#0x190]
1083f42c  0a0d80e2  add r0, r0, #0x280
1083f430  236a00eb  bl 0x10859cc4
1083f434  0600a0e1  cpy r0, r6
1083f438  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
