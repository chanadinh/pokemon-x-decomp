0013a50c  70402de9  stmdb sp!,{r4,r5,r6,lr}
0013a510  0040b0e1  movs r4, r0
0013a514  2200000a  beq 0x0013a5a4
0013a518  200014e5  ldr r0, [r4,#-0x20]
0013a51c  58c3ffeb  bl 0x0012b284
0013a520  0050a0e1  cpy r5, r0
0013a524  000090e5  ldr r0, [r0,#0x0]
0013a528  206044e2  sub r6, r4, #0x20
0013a52c  201090e5  ldr r1, [r0,#0x20]
0013a530  0500a0e1  cpy r0, r5
0013a534  31ff2fe1  blx r1
0013a538  0010a0e1  cpy r1, r0
0013a53c  000096e5  ldr r0, [r6,#0x0]
0013a540  000051e1  cmp r1, r0
0013a544  0b00000a  beq 0x0013a578
0013a548  4dc3ffeb  bl 0x0012b284
0013a54c  002090e5  ldr r2, [r0,#0x0]
0013a550  bc1154e1  ldrh r1, [r4,#-0x1c]
0013a554  0050a0e1  cpy r5, r0
0013a558  5c2092e5  ldr r2, [r2,#0x5c]
0013a55c  011044e0  sub r1, r4, r1
0013a560  32ff2fe1  blx r2
0013a564  000095e5  ldr r0, [r5,#0x0]
0013a568  4c1090e5  ldr r1, [r0,#0x4c]
0013a56c  0500a0e1  cpy r0, r5
0013a570  7040bde8  ldmia sp!,{r4,r5,r6,lr}
0013a574  11ff2fe1  bx r1
0013a578  000095e5  ldr r0, [r5,#0x0]
0013a57c  4c1090e5  ldr r1, [r0,#0x4c]
0013a580  0500a0e1  cpy r0, r5
0013a584  31ff2fe1  blx r1
0013a588  bc0154e1  ldrh r0, [r4,#-0x1c]
0013a58c  001044e0  sub r1, r4, r0
0013a590  000095e5  ldr r0, [r5,#0x0]
0013a594  5c2090e5  ldr r2, [r0,#0x5c]
0013a598  0500a0e1  cpy r0, r5
0013a59c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
0013a5a0  12ff2fe1  bx r2
0013a5a4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
