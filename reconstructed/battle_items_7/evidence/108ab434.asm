108ab434  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108ab438  0060a0e1  cpy r6, r0
108ab43c  0180a0e1  cpy r8, r1
108ab440  0250a0e1  cpy r5, r2
108ab444  0200a0e3  mov r0, #0x2
108ab448  3301ffeb  bl 0x1086b91c
108ab44c  050050e1  cmp r0, r5
108ab450  2e00001a  bne 0x108ab510
108ab454  0510a0e1  cpy r1, r5
108ab458  0800a0e1  cpy r0, r8
108ab45c  0fa4feeb  bl 0x108544a0
108ab460  0090a0e1  cpy r9, r0
108ab464  0520a0e1  cpy r2, r5
108ab468  0610a0e3  mov r1, #0x6
108ab46c  0800a0e1  cpy r0, r8
108ab470  c1fafeeb  bl 0x10869f7c
108ab474  0040a0e1  cpy r4, r0
108ab478  0650c0e5  strb r5, [r0,#0x6]
108ab47c  0600a0e1  cpy r0, r6
108ab480  0370a0e3  mov r7, #0x3
108ab484  b803d0e1  ldrh r0, [r0,#0x38]
108ab488  0710a0e1  cpy r1, r7
108ab48c  b886feeb  bl 0x1084cf74
108ab490  0010a0e1  cpy r1, r0
108ab494  0900a0e1  cpy r0, r9
108ab498  9587feeb  bl 0x1084d2f4
108ab49c  0010a0e1  cpy r1, r0
108ab4a0  0720d4e5  ldrb r2, [r4,#0x7]
108ab4a4  b410c4e1  strh r1, [r4,#0x4]
108ab4a8  9b0f87e2  add r0, r7, #0x26c
108ab4ac  b800c4e1  strh r0, [r4,#0x8]
108ab4b0  021082e3  orr r1, r2, #0x2
108ab4b4  0710c4e5  strb r1, [r4,#0x7]
108ab4b8  0510a0e1  cpy r1, r5
108ab4bc  0800a0e1  cpy r0, r8
108ab4c0  241fffeb  bl 0x10873158
108ab4c4  0a00c4e5  strb r0, [r4,#0xa]
108ab4c8  0a10a0e3  mov r1, #0xa
108ab4cc  0b10c4e5  strb r1, [r4,#0xb]
108ab4d0  3c209fe5  ldr r2, [0x108ab514]
108ab4d4  0210a0e3  mov r1, #0x2
108ab4d8  0c0084e2  add r0, r4, #0xc
108ab4dc  7c87feeb  bl 0x1084d2d4
108ab4e0  0510a0e1  cpy r1, r5
108ab4e4  0c0084e2  add r0, r4, #0xc
108ab4e8  6b87feeb  bl 0x1084d29c
108ab4ec  0600a0e1  cpy r0, r6
108ab4f0  b803d0e1  ldrh r0, [r0,#0x38]
108ab4f4  0010a0e1  cpy r1, r0
108ab4f8  0c0084e2  add r0, r4, #0xc
108ab4fc  6687feeb  bl 0x1084d29c
108ab500  0410a0e1  cpy r1, r4
108ab504  0800a0e1  cpy r0, r8
108ab508  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108ab50c  8df1feea  b 0x10867b48
108ab510  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
