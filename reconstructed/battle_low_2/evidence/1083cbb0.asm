1083cbb0  6c209fe5  ldr r2, [0x1083cc24]
1083cbb4  70402de9  stmdb sp!,{r4,r5,r6,lr}
1083cbb8  0060a0e3  mov r6, #0x0
1083cbbc  64109fe5  ldr r1, [0x1083cc28]
1083cbc0  6c3092e5  ldr r3, [r2,#0x6c]
1083cbc4  010053e1  cmp r3, r1
1083cbc8  6c108215  strne r1, [r2,#0x6c]
1083cbcc  904190e5  ldr r4, [r0,#0x190]
1083cbd0  0600a0e1  cpy r0, r6
1083cbd4  235e84e2  add r5, r4, #0x230
1083cbd8  3c6284e5  str r6, [r4,#0x23c]
1083cbdc  4462c4e5  strb r6, [r4,#0x244]
1083cbe0  416700eb  bl 0x108568ec
1083cbe4  080095e5  ldr r0, [r5,#0x8]
1083cbe8  0110a0e3  mov r1, #0x1
1083cbec  667500eb  bl 0x1085a18c
1083cbf0  880094e5  ldr r0, [r4,#0x88]
1083cbf4  001090e5  ldr r1, [r0,#0x0]
1083cbf8  102091e5  ldr r2, [r1,#0x10]
1083cbfc  0010a0e3  mov r1, #0x0
1083cc00  32ff2fe1  blx r2
1083cc04  886084e5  str r6, [r4,#0x88]
1083cc08  0000a0e3  mov r0, #0x0
1083cc0c  9360c4e5  strb r6, [r4,#0x93]
1083cc10  356700eb  bl 0x108568ec
1083cc14  740094e5  ldr r0, [r4,#0x74]
1083cc18  7040bde8  ldmia sp!,{r4,r5,r6,lr}
1083cc1c  0110a0e3  mov r1, #0x1
1083cc20  597500ea  b 0x1085a18c
