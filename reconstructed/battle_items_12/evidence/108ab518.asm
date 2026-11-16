108ab518  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab51c  0050a0e1  cpy r5, r0
108ab520  0160a0e1  cpy r6, r1
108ab524  0240a0e1  cpy r4, r2
108ab528  0c00a0e3  mov r0, #0xc
108ab52c  fa00ffeb  bl 0x1086b91c
108ab530  060050e3  cmp r0, #0x6
108ab534  1800000a  beq 0x108ab59c
108ab538  0200a0e3  mov r0, #0x2
108ab53c  f600ffeb  bl 0x1086b91c
108ab540  040050e1  cmp r0, r4
108ab544  1400001a  bne 0x108ab59c
108ab548  0410a0e1  cpy r1, r4
108ab54c  0600a0e1  cpy r0, r6
108ab550  d2a3feeb  bl 0x108544a0
108ab554  0500a0e1  cpy r0, r5
108ab558  0370a0e3  mov r7, #0x3
108ab55c  b803d0e1  ldrh r0, [r0,#0x38]
108ab560  0710a0e1  cpy r1, r7
108ab564  8286feeb  bl 0x1084cf74
108ab568  ff2000e2  and r2, r0, #0xff
108ab56c  0410a0e1  cpy r1, r4
108ab570  0600a0e1  cpy r0, r6
108ab574  6d84feeb  bl 0x1084c730
108ab578  000050e3  cmp r0, #0x0
108ab57c  0600000a  beq 0x108ab59c
108ab580  0500a0e1  cpy r0, r5
108ab584  f884feeb  bl 0x1084c96c
108ab588  0420a0e1  cpy r2, r4
108ab58c  0600a0e1  cpy r0, r6
108ab590  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab594  0010a0e3  mov r1, #0x0
108ab598  ea84feea  b 0x1084c948
108ab59c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
