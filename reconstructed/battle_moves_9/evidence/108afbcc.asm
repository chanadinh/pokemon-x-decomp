108afbcc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108afbd0  0050a0e1  cpy r5, r0
108afbd4  0160a0e1  cpy r6, r1
108afbd8  0240a0e1  cpy r4, r2
108afbdc  0200a0e3  mov r0, #0x2
108afbe0  4deffeeb  bl 0x1086b91c
108afbe4  040050e1  cmp r0, r4
108afbe8  1600001a  bne 0x108afc48
108afbec  0600a0e1  cpy r0, r6
108afbf0  200080e2  add r0, r0, #0x20
108afbf4  0070a0e1  cpy r7, r0
108afbf8  0500a0e1  cpy r0, r5
108afbfc  b803d0e1  ldrh r0, [r0,#0x38]
108afc00  0050a0e1  cpy r5, r0
108afc04  0600a0e1  cpy r0, r6
108afc08  d575feeb  bl 0x1084d364
108afc0c  0020a0e1  cpy r2, r0
108afc10  0510a0e1  cpy r1, r5
108afc14  0700a0e1  cpy r0, r7
108afc18  aa6700eb  bl 0x108c9ac8
108afc1c  010050e3  cmp r0, #0x1
108afc20  0800001a  bne 0x108afc48
108afc24  0420a0e1  cpy r2, r4
108afc28  3110a0e3  mov r1, #0x31
108afc2c  0600a0e1  cpy r0, r6
108afc30  d1e8feeb  bl 0x10869f7c
108afc34  0010a0e1  cpy r1, r0
108afc38  b450c0e1  strh r5, [r0,#0x4]
108afc3c  0600a0e1  cpy r0, r6
108afc40  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108afc44  bfdffeea  b 0x10867b48
108afc48  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
