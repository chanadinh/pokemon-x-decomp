108aaec4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aaec8  0250a0e1  cpy r5, r2
108aaecc  0080a0e1  cpy r8, r0
108aaed0  0170a0e1  cpy r7, r1
108aaed4  0500a0e3  mov r0, #0x5
108aaed8  8f02ffeb  bl 0x1086b91c
108aaedc  006050e2  subs r6, r0, #0x0
108aaee0  0040a0e3  mov r4, #0x0
108aaee4  1d00009a  bls 0x108aaf60
108aaee8  060084e2  add r0, r4, #0x6
108aaeec  ff0000e2  and r0, r0, #0xff
108aaef0  8902ffeb  bl 0x1086b91c
108aaef4  050050e1  cmp r0, r5
108aaef8  00f020e3  nop
108aaefc  1400001a  bne 0x108aaf54
108aaf00  1b00a0e3  mov r0, #0x1b
108aaf04  8402ffeb  bl 0x1086b91c
108aaf08  010050e3  cmp r0, #0x1
108aaf0c  00f020e3  nop
108aaf10  1200001a  bne 0x108aaf60
108aaf14  0510a0e1  cpy r1, r5
108aaf18  0700a0e1  cpy r0, r7
108aaf1c  5fa5feeb  bl 0x108544a0
108aaf20  0120a0e3  mov r2, #0x1
108aaf24  0210a0e3  mov r1, #0x2
108aaf28  ee1cffeb  bl 0x108722e8
108aaf2c  000050e3  cmp r0, #0x0
108aaf30  00f020e3  nop
108aaf34  0900000a  beq 0x108aaf60
108aaf38  0800a0e1  cpy r0, r8
108aaf3c  8a86feeb  bl 0x1084c96c
108aaf40  0520a0e1  cpy r2, r5
108aaf44  0700a0e1  cpy r0, r7
108aaf48  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aaf4c  0010a0e3  mov r1, #0x0
108aaf50  7c86feea  b 0x1084c948
108aaf54  014084e2  add r4, r4, #0x1
108aaf58  040056e1  cmp r6, r4
108aaf5c  e1ffff8a  bhi 0x108aaee8
108aaf60  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
