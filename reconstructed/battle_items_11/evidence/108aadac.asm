108aadac  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aadb0  0250a0e1  cpy r5, r2
108aadb4  0080a0e1  cpy r8, r0
108aadb8  0170a0e1  cpy r7, r1
108aadbc  0500a0e3  mov r0, #0x5
108aadc0  d502ffeb  bl 0x1086b91c
108aadc4  006050e2  subs r6, r0, #0x0
108aadc8  0040a0e3  mov r4, #0x0
108aadcc  1d00009a  bls 0x108aae48
108aadd0  060084e2  add r0, r4, #0x6
108aadd4  ff0000e2  and r0, r0, #0xff
108aadd8  cf02ffeb  bl 0x1086b91c
108aaddc  050050e1  cmp r0, r5
108aade0  00f020e3  nop
108aade4  1400001a  bne 0x108aae3c
108aade8  1b00a0e3  mov r0, #0x1b
108aadec  ca02ffeb  bl 0x1086b91c
108aadf0  020050e3  cmp r0, #0x2
108aadf4  00f020e3  nop
108aadf8  1200001a  bne 0x108aae48
108aadfc  0510a0e1  cpy r1, r5
108aae00  0700a0e1  cpy r0, r7
108aae04  a5a5feeb  bl 0x108544a0
108aae08  0120a0e3  mov r2, #0x1
108aae0c  0410a0e3  mov r1, #0x4
108aae10  341dffeb  bl 0x108722e8
108aae14  000050e3  cmp r0, #0x0
108aae18  00f020e3  nop
108aae1c  0900000a  beq 0x108aae48
108aae20  0800a0e1  cpy r0, r8
108aae24  d086feeb  bl 0x1084c96c
108aae28  0520a0e1  cpy r2, r5
108aae2c  0700a0e1  cpy r0, r7
108aae30  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aae34  0010a0e3  mov r1, #0x0
108aae38  c286feea  b 0x1084c948
108aae3c  014084e2  add r4, r4, #0x1
108aae40  040056e1  cmp r6, r4
108aae44  e1ffff8a  bhi 0x108aadd0
108aae48  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
