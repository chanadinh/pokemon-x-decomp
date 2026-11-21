108b3be8  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108b3bec  0300a0e3  mov r0, #0x3
108b3bf0  0180a0e1  cpy r8, r1
108b3bf4  0270a0e1  cpy r7, r2
108b3bf8  47dffeeb  bl 0x1086b91c
108b3bfc  070050e1  cmp r0, r7
108b3c00  1700001a  bne 0x108b3c64
108b3c04  0500a0e3  mov r0, #0x5
108b3c08  43dffeeb  bl 0x1086b91c
108b3c0c  006050e2  subs r6, r0, #0x0
108b3c10  50909f85  ldrhi r9, [0x108b3c68]
108b3c14  0040a0e3  mov r4, #0x0
108b3c18  01a0a083  movhi r10, #0x1
108b3c1c  1000009a  bls 0x108b3c64
108b3c20  0720a0e1  cpy r2, r7
108b3c24  1310a0e3  mov r1, #0x13
108b3c28  0800a0e1  cpy r0, r8
108b3c2c  d2d8feeb  bl 0x10869f7c
108b3c30  0050a0e1  cpy r5, r0
108b3c34  b490c0e1  strh r9, [r0,#0x4]
108b3c38  060084e2  add r0, r4, #0x6
108b3c3c  ff0000e2  and r0, r0, #0xff
108b3c40  35dffeeb  bl 0x1086b91c
108b3c44  0600c5e5  strb r0, [r5,#0x6]
108b3c48  0510a0e1  cpy r1, r5
108b3c4c  0800a0e1  cpy r0, r8
108b3c50  08a0c5e5  strb r10, [r5,#0x8]
108b3c54  bbcffeeb  bl 0x10867b48
108b3c58  014084e2  add r4, r4, #0x1
108b3c5c  040056e1  cmp r6, r4
108b3c60  eeffff8a  bhi 0x108b3c20
108b3c64  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
