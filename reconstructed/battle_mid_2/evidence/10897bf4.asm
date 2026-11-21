10897bf4  70402de9  stmdb sp!,{r4,r5,r6,lr}
10897bf8  0040a0e1  cpy r4, r0
10897bfc  0000a0e3  mov r0, #0x0
10897c00  2400c4e5  strb r0, [r4,#0x24]
10897c04  0200a0e3  mov r0, #0x2
10897c08  d500c4e5  strb r0, [r4,#0xd5]
10897c0c  e00094e5  ldr r0, [r4,#0xe0]
10897c10  0050a0e3  mov r5, #0x0
10897c14  0360a0e3  mov r6, #0x3
10897c18  55a6fdeb  bl 0x10801574
10897c1c  e850c4e5  strb r5, [r4,#0xe8]
10897c20  1000a0e3  mov r0, #0x10
10897c24  e960c4e5  strb r6, [r4,#0xe9]
10897c28  ea00c4e5  strb r0, [r4,#0xea]
10897c2c  e40094e5  ldr r0, [r4,#0xe4]
10897c30  0010a0e3  mov r1, #0x0
10897c34  6ca6fdeb  bl 0x108015ec
10897c38  0100a0e3  mov r0, #0x1
10897c3c  2afbfeeb  bl 0x108568ec
10897c40  d80094e5  ldr r0, [r4,#0xd8]
10897c44  7040bde8  ldmia sp!,{r4,r5,r6,lr}
10897c48  0f10a0e3  mov r1, #0xf
10897c4c  33f0feea  b 0x10853d20
