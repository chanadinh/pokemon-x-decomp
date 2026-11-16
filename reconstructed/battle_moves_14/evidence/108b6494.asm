108b6494  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b6498  0160a0e1  cpy r6, r1
108b649c  0070a0e1  cpy r7, r0
108b64a0  0250a0e1  cpy r5, r2
108b64a4  0340a0e1  cpy r4, r3
108b64a8  0200a0e3  mov r0, #0x2
108b64ac  1ad5feeb  bl 0x1086b91c
108b64b0  050050e1  cmp r0, r5
108b64b4  1900001a  bne 0x108b6520
108b64b8  000094e5  ldr r0, [r4,#0x0]
108b64bc  000050e3  cmp r0, #0x0
108b64c0  1600000a  beq 0x108b6520
108b64c4  0520a0e1  cpy r2, r5
108b64c8  0310a0e3  mov r1, #0x3
108b64cc  0600a0e1  cpy r0, r6
108b64d0  a9cefeeb  bl 0x10869f7c
108b64d4  0040a0e1  cpy r4, r0
108b64d8  44209fe5  ldr r2, [0x108b6524]
108b64dc  0210a0e3  mov r1, #0x2
108b64e0  040080e2  add r0, r0, #0x4
108b64e4  7a5bfeeb  bl 0x1084d2d4
108b64e8  0510a0e1  cpy r1, r5
108b64ec  040084e2  add r0, r4, #0x4
108b64f0  695bfeeb  bl 0x1084d29c
108b64f4  0300a0e3  mov r0, #0x3
108b64f8  07d5feeb  bl 0x1086b91c
108b64fc  0010a0e1  cpy r1, r0
108b6500  040084e2  add r0, r4, #0x4
108b6504  645bfeeb  bl 0x1084d29c
108b6508  0410a0e1  cpy r1, r4
108b650c  0600a0e1  cpy r0, r6
108b6510  8cc5feeb  bl 0x10867b48
108b6514  0700a0e1  cpy r0, r7
108b6518  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b651c  2a5bfeea  b 0x1084d1cc
108b6520  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
