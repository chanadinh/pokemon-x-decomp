0012acd8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0012acdc  0040a0e1  cpy r4, r0
0012ace0  0070a0e3  mov r7, #0x0
0012ace4  5c009fe5  ldr r0, [0x12ad48]
0012ace8  0250a0e1  cpy r5, r2
0012acec  000084e5  str r0, [r4,#0x0]
0012acf0  7100ffe6  uxth r0, r1
0012acf4  0c70c4e5  strb r7, [r4,#0xc]
0012acf8  ba70c4e1  strh r7, [r4,#0xa]
0012acfc  8060a0e1  mov r6, r0, lsl #0x1
0012ad00  b800c4e1  strh r0, [r4,#0x8]
0012ad04  000092e5  ldr r0, [r2,#0x0]
0012ad08  1c1090e5  ldr r1, [r0,#0x1c]
0012ad0c  0200a0e1  cpy r0, r2
0012ad10  31ff2fe1  blx r1
0012ad14  0020a0e1  cpy r2, r0
0012ad18  0130a0e3  mov r3, #0x1
0012ad1c  0610a0e1  cpy r1, r6
0012ad20  0500a0e1  cpy r0, r5
0012ad24  e70000eb  bl 0x0012b0c8
0012ad28  000050e3  cmp r0, #0x0
0012ad2c  040084e5  str r0, [r4,#0x4]
0012ad30  0200000a  beq 0x0012ad40
0012ad34  0110a0e3  mov r1, #0x1
0012ad38  b070c0e1  strh r7, [r0,#0x0]
0012ad3c  0c10c4e5  strb r1, [r4,#0xc]
0012ad40  0400a0e1  cpy r0, r4
0012ad44  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
