1084aca8  0100a0e1  cpy r0, r1
1084acac  0210a0e1  cpy r1, r2
1084acb0  473fa0e3  mov r3, #0x11c
1084acb4  0120a0e3  mov r2, #0x1
1084acb8  0000a0e1  cpy r0, r0
1084acbc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1084acc0  0160a0e1  cpy r6, r1
1084acc4  0080a0e1  cpy r8, r0
1084acc8  0270a0e1  cpy r7, r2
1084accc  0350a0e1  cpy r5, r3
1084acd0  0200a0e3  mov r0, #0x2
1084acd4  108300eb  bl 0x1086b91c
1084acd8  060050e1  cmp r0, r6
1084acdc  1a00001a  bne 0x1084ad4c
1084ace0  000055e3  cmp r5, #0x0
1084ace4  0540a0e3  mov r4, #0x5
1084ace8  0a00000a  beq 0x1084ad18
1084acec  0610a0e1  cpy r1, r6
1084acf0  0800a0e1  cpy r0, r8
1084acf4  e92500eb  bl 0x108544a0
1084acf8  b201d0e1  ldrh r0, [r0,#0x12]
1084acfc  050050e1  cmp r0, r5
1084ad00  0400001a  bne 0x1084ad18
1084ad04  0610a0e1  cpy r1, r6
1084ad08  0800a0e1  cpy r0, r8
1084ad0c  290200eb  bl 0x1084b5b8
1084ad10  000050e3  cmp r0, #0x0
1084ad14  0840a013  movne r4, #0x8
1084ad18  0620a0e1  cpy r2, r6
1084ad1c  1e10a0e3  mov r1, #0x1e
1084ad20  0800a0e1  cpy r0, r8
1084ad24  947c00eb  bl 0x10869f7c
1084ad28  0010a0e1  cpy r1, r0
1084ad2c  000090e5  ldr r0, [r0,#0x0]
1084ad30  0470c1e5  strb r7, [r1,#0x4]
1084ad34  0540c1e5  strb r4, [r1,#0x5]
1084ad38  020580e3  orr r0, r0, #0x800000
1084ad3c  000081e5  str r0, [r1,#0x0]
1084ad40  0800a0e1  cpy r0, r8
1084ad44  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1084ad48  7e7300ea  b 0x10867b48
1084ad4c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
