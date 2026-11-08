108b7b94  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b7b98  0260a0e1  cpy r6, r2
108b7b9c  0080a0e1  cpy r8, r0
108b7ba0  0170a0e1  cpy r7, r1
108b7ba4  0340a0e1  cpy r4, r3
108b7ba8  0200a0e3  mov r0, #0x2
108b7bac  5acffeeb  bl 0x1086b91c
108b7bb0  060050e1  cmp r0, r6
108b7bb4  2600001a  bne 0x108b7c54
108b7bb8  0610a0e1  cpy r1, r6
108b7bbc  0700a0e1  cpy r0, r7
108b7bc0  3672feeb  bl 0x108544a0
108b7bc4  0050a0e1  cpy r5, r0
108b7bc8  1910a0e3  mov r1, #0x19
108b7bcc  ad7bfeeb  bl 0x10856a88
108b7bd0  000050e3  cmp r0, #0x0
108b7bd4  1e00001a  bne 0x108b7c54
108b7bd8  0210a0e3  mov r1, #0x2
108b7bdc  0500a0e1  cpy r0, r5
108b7be0  a87bfeeb  bl 0x10856a88
108b7be4  000050e3  cmp r0, #0x0
108b7be8  18009405  ldreq r0, [r4,#0x18]
108b7bec  00005003  cmpeq r0, #0x0
108b7bf0  1700001a  bne 0x108b7c54
108b7bf4  0620a0e1  cpy r2, r6
108b7bf8  0b10a0e3  mov r1, #0xb
108b7bfc  0700a0e1  cpy r0, r7
108b7c00  ddc8feeb  bl 0x10869f7c
108b7c04  0050a0e1  cpy r5, r0
108b7c08  1900a0e3  mov r0, #0x19
108b7c0c  b400c5e1  strh r0, [r5,#0x4]
108b7c10  0800a0e1  cpy r0, r8
108b7c14  b803d0e1  ldrh r0, [r0,#0x38]
108b7c18  0010a0e1  cpy r1, r0
108b7c1c  0500a0e3  mov r0, #0x5
108b7c20  a150feeb  bl 0x1084beac
108b7c24  080085e5  str r0, [r5,#0x8]
108b7c28  0080a0e3  mov r8, #0x0
108b7c2c  0c80c5e5  strb r8, [r5,#0xc]
108b7c30  0510a0e1  cpy r1, r5
108b7c34  0700a0e1  cpy r0, r7
108b7c38  0f60c5e5  strb r6, [r5,#0xf]
108b7c3c  c1bffeeb  bl 0x10867b48
108b7c40  0100a0e3  mov r0, #0x1
108b7c44  0510a0e3  mov r1, #0x5
108b7c48  180084e5  str r0, [r4,#0x18]
108b7c4c  041084e5  str r1, [r4,#0x4]
108b7c50  008084e5  str r8, [r4,#0x0]
108b7c54  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
