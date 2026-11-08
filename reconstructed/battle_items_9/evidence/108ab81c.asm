108ab81c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ab820  0160a0e1  cpy r6, r1
108ab824  0070a0e1  cpy r7, r0
108ab828  0250a0e1  cpy r5, r2
108ab82c  0200a0e3  mov r0, #0x2
108ab830  3900ffeb  bl 0x1086b91c
108ab834  050050e1  cmp r0, r5
108ab838  1400001a  bne 0x108ab890
108ab83c  0520a0e1  cpy r2, r5
108ab840  0310a0e3  mov r1, #0x3
108ab844  0600a0e1  cpy r0, r6
108ab848  cbf9feeb  bl 0x10869f7c
108ab84c  0040a0e1  cpy r4, r0
108ab850  3c209fe5  ldr r2, [0x108ab894]
108ab854  0210a0e3  mov r1, #0x2
108ab858  040080e2  add r0, r0, #0x4
108ab85c  9c86feeb  bl 0x1084d2d4
108ab860  0510a0e1  cpy r1, r5
108ab864  040084e2  add r0, r4, #0x4
108ab868  8b86feeb  bl 0x1084d29c
108ab86c  0700a0e1  cpy r0, r7
108ab870  b803d0e1  ldrh r0, [r0,#0x38]
108ab874  0010a0e1  cpy r1, r0
108ab878  040084e2  add r0, r4, #0x4
108ab87c  8686feeb  bl 0x1084d29c
108ab880  0410a0e1  cpy r1, r4
108ab884  0600a0e1  cpy r0, r6
108ab888  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ab88c  adf0feea  b 0x10867b48
108ab890  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
