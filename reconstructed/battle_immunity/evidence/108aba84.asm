108aba84  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aba88  0040a0e1  cpy r4, r0
108aba8c  0160a0e1  cpy r6, r1
108aba90  0250a0e1  cpy r5, r2
108aba94  0400a0e3  mov r0, #0x4
108aba98  9ffffeeb  bl 0x1086b91c
108aba9c  050050e1  cmp r0, r5
108abaa0  2600001a  bne 0x108abb40
108abaa4  1200a0e3  mov r0, #0x12
108abaa8  9bfffeeb  bl 0x1086b91c
108abaac  7000ffe6  uxth r0, r0
108abab0  4fe3ffeb  bl 0x108a47f4
108abab4  000050e3  cmp r0, #0x0
108abab8  2000000a  beq 0x108abb40
108ababc  0110a0e3  mov r1, #0x1
108abac0  4100a0e3  mov r0, #0x41
108abac4  82e7feeb  bl 0x108658d4
108abac8  000050e3  cmp r0, #0x0
108abacc  1b00000a  beq 0x108abb40
108abad0  0400a0e1  cpy r0, r4
108abad4  b803d0e1  ldrh r0, [r0,#0x38]
108abad8  0070a0e1  cpy r7, r0
108abadc  1200a0e3  mov r0, #0x12
108abae0  8dfffeeb  bl 0x1086b91c
108abae4  0080a0e1  cpy r8, r0
108abae8  0520a0e1  cpy r2, r5
108abaec  0310a0e3  mov r1, #0x3
108abaf0  0600a0e1  cpy r0, r6
108abaf4  20f9feeb  bl 0x10869f7c
108abaf8  0040a0e1  cpy r4, r0
108abafc  40209fe5  ldr r2, [0x108abb44]
108abb00  0210a0e3  mov r1, #0x2
108abb04  040080e2  add r0, r0, #0x4
108abb08  f185feeb  bl 0x1084d2d4
108abb0c  0510a0e1  cpy r1, r5
108abb10  040084e2  add r0, r4, #0x4
108abb14  e085feeb  bl 0x1084d29c
108abb18  0710a0e1  cpy r1, r7
108abb1c  040084e2  add r0, r4, #0x4
108abb20  dd85feeb  bl 0x1084d29c
108abb24  0810a0e1  cpy r1, r8
108abb28  040084e2  add r0, r4, #0x4
108abb2c  da85feeb  bl 0x1084d29c
108abb30  0410a0e1  cpy r1, r4
108abb34  0600a0e1  cpy r0, r6
108abb38  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108abb3c  01f0feea  b 0x10867b48
108abb40  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
