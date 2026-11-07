1084cc48  f8432de9  stmdb sp!,{r3,r4,r5,r6,r7,r8,r9,lr}
1084cc4c  0150a0e1  cpy r5, r1
1084cc50  0070a0e1  cpy r7, r0
1084cc54  0240a0e1  cpy r4, r2
1084cc58  0360a0e1  cpy r6, r3
1084cc5c  0200a0e3  mov r0, #0x2
1084cc60  2d7b00eb  bl 0x1086b91c
1084cc64  040050e1  cmp r0, r4
1084cc68  1c00001a  bne 0x1084cce0
1084cc6c  0d10a0e1  cpy r1, sp
1084cc70  2f00a0e3  mov r0, #0x2f
1084cc74  c9bc00eb  bl 0x1087bfa0
1084cc78  000050e3  cmp r0, #0x0
1084cc7c  0200000a  beq 0x1084cc8c
1084cc80  00009de5  ldr r0, [sp,#0x0]
1084cc84  040010e3  tst r0, #0x4
1084cc88  1400000a  beq 0x1084cce0
1084cc8c  0500a0e1  cpy r0, r5
1084cc90  0410a0e1  cpy r1, r4
1084cc94  011e00eb  bl 0x108544a0
1084cc98  260056e3  cmp r6, #0x26
1084cc9c  0080a0e1  cpy r8, r0
1084cca0  1b00000a  beq 0x1084cd14
1084cca4  270056e3  cmp r6, #0x27
1084cca8  0d00000a  beq 0x1084cce4
1084ccac  280056e3  cmp r6, #0x28
1084ccb0  1d00000a  beq 0x1084cd2c
1084ccb4  0610a0e1  cpy r1, r6
1084ccb8  722700eb  bl 0x10856a88
1084ccbc  000050e3  cmp r0, #0x0
1084ccc0  00f020e3  nop
1084ccc4  0500000a  beq 0x1084cce0
1084ccc8  0700a0e1  cpy r0, r7
1084cccc  26ffffeb  bl 0x1084c96c
1084ccd0  0420a0e1  cpy r2, r4
1084ccd4  0010a0e3  mov r1, #0x0
1084ccd8  0500a0e1  cpy r0, r5
1084ccdc  19ffffeb  bl 0x1084c948
1084cce0  f883bde8  ldmia sp!,{r3,r4,r5,r6,r7,r8,r9,pc}
1084cce4  00f020e3  nop
1084cce8  ef8200eb  bl 0x1086d8ac
1084ccec  000050e3  cmp r0, #0x0
1084ccf0  00f020e3  nop
1084ccf4  f3ffff1a  bne 0x1084ccc8
1084ccf8  0610a0e3  mov r1, #0x6
1084ccfc  0800a0e1  cpy r0, r8
1084cd00  602700eb  bl 0x10856a88
1084cd04  000050e3  cmp r0, #0x0
1084cd08  00f020e3  nop
1084cd0c  edffff1a  bne 0x1084ccc8
1084cd10  f883bde8  ldmia sp!,{r3,r4,r5,r6,r7,r8,r9,pc}
1084cd14  00f020e3  nop
1084cd18  e38200eb  bl 0x1086d8ac
1084cd1c  000050e3  cmp r0, #0x0
1084cd20  00f020e3  nop
1084cd24  e7ffff1a  bne 0x1084ccc8
1084cd28  f883bde8  ldmia sp!,{r3,r4,r5,r6,r7,r8,r9,pc}
1084cd2c  00f020e3  nop
1084cd30  c3feffeb  bl 0x1084c844
1084cd34  000050e3  cmp r0, #0x0
1084cd38  00f020e3  nop
1084cd3c  e1ffff1a  bne 0x1084ccc8
1084cd40  f883bde8  ldmia sp!,{r3,r4,r5,r6,r7,r8,r9,pc}
