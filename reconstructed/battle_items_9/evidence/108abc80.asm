108abc80  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108abc84  0050a0e1  cpy r5, r0
108abc88  0140a0e1  cpy r4, r1
108abc8c  0260a0e1  cpy r6, r2
108abc90  0200a0e3  mov r0, #0x2
108abc94  20fffeeb  bl 0x1086b91c
108abc98  060050e1  cmp r0, r6
108abc9c  1a00001a  bne 0x108abd0c
108abca0  0500a0e1  cpy r0, r5
108abca4  0370a0e3  mov r7, #0x3
108abca8  b803d0e1  ldrh r0, [r0,#0x38]
108abcac  0710a0e1  cpy r1, r7
108abcb0  af84feeb  bl 0x1084cf74
108abcb4  ff0000e2  and r0, r0, #0xff
108abcb8  0e81feeb  bl 0x1084c0f8
108abcbc  000050e3  cmp r0, #0x0
108abcc0  0400001a  bne 0x108abcd8
108abcc4  0210a0e3  mov r1, #0x2
108abcc8  0400a0e1  cpy r0, r4
108abccc  0681feeb  bl 0x1084c0ec
108abcd0  000050e3  cmp r0, #0x0
108abcd4  0c00000a  beq 0x108abd0c
108abcd8  0210a0e3  mov r1, #0x2
108abcdc  1100a0e3  mov r0, #0x11
108abce0  fbe6feeb  bl 0x108658d4
108abce4  000050e3  cmp r0, #0x0
108abce8  00f020e3  nop
108abcec  0600000a  beq 0x108abd0c
108abcf0  0500a0e1  cpy r0, r5
108abcf4  1c83feeb  bl 0x1084c96c
108abcf8  0620a0e1  cpy r2, r6
108abcfc  0400a0e1  cpy r0, r4
108abd00  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108abd04  0010a0e3  mov r1, #0x0
108abd08  0e83feea  b 0x1084c948
108abd0c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
