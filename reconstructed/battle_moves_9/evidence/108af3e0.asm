108af3e0  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108af3e4  0260a0e1  cpy r6, r2
108af3e8  0180a0e1  cpy r8, r1
108af3ec  0090a0e1  cpy r9, r0
108af3f0  0350a0e1  cpy r5, r3
108af3f4  0200a0e3  mov r0, #0x2
108af3f8  47f1feeb  bl 0x1086b91c
108af3fc  060050e1  cmp r0, r6
108af400  2200001a  bne 0x108af490
108af404  0610a0e1  cpy r1, r6
108af408  0800a0e1  cpy r0, r8
108af40c  2394feeb  bl 0x108544a0
108af410  1910a0e3  mov r1, #0x19
108af414  9b9dfeeb  bl 0x10856a88
108af418  000050e3  cmp r0, #0x0
108af41c  18009505  ldreq r0, [r5,#0x18]
108af420  00005003  cmpeq r0, #0x0
108af424  1900001a  bne 0x108af490
108af428  0200a0e3  mov r0, #0x2
108af42c  e274feeb  bl 0x1084c7bc
108af430  020080e2  add r0, r0, #0x2
108af434  ff7000e2  and r7, r0, #0xff
108af438  0620a0e1  cpy r2, r6
108af43c  0b10a0e3  mov r1, #0xb
108af440  0800a0e1  cpy r0, r8
108af444  cceafeeb  bl 0x10869f7c
108af448  0040a0e1  cpy r4, r0
108af44c  1900a0e3  mov r0, #0x19
108af450  b400c4e1  strh r0, [r4,#0x4]
108af454  0900a0e1  cpy r0, r9
108af458  b803d0e1  ldrh r0, [r0,#0x38]
108af45c  0010a0e1  cpy r1, r0
108af460  0700a0e1  cpy r0, r7
108af464  9072feeb  bl 0x1084beac
108af468  080084e5  str r0, [r4,#0x8]
108af46c  0000a0e3  mov r0, #0x0
108af470  0c00c4e5  strb r0, [r4,#0xc]
108af474  0410a0e1  cpy r1, r4
108af478  0800a0e1  cpy r0, r8
108af47c  0f60c4e5  strb r6, [r4,#0xf]
108af480  b0e1feeb  bl 0x10867b48
108af484  0100a0e3  mov r0, #0x1
108af488  180085e5  str r0, [r5,#0x18]
108af48c  007085e5  str r7, [r5,#0x0]
108af490  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
