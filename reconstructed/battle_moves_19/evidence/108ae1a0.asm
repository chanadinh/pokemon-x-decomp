108ae1a0  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108ae1a4  0240a0e1  cpy r4, r2
108ae1a8  0180a0e1  cpy r8, r1
108ae1ac  0300a0e3  mov r0, #0x3
108ae1b0  d9f5feeb  bl 0x1086b91c
108ae1b4  040050e1  cmp r0, r4
108ae1b8  2f00001a  bne 0x108ae27c
108ae1bc  0600a0e3  mov r0, #0x6
108ae1c0  d5f5feeb  bl 0x1086b91c
108ae1c4  ff7000e2  and r7, r0, #0xff
108ae1c8  0710a0e1  cpy r1, r7
108ae1cc  0800a0e1  cpy r0, r8
108ae1d0  b298feeb  bl 0x108544a0
108ae1d4  0050a0e1  cpy r5, r0
108ae1d8  2678feeb  bl 0x1084c278
108ae1dc  0090a0e1  cpy r9, r0
108ae1e0  0010a0e1  cpy r1, r0
108ae1e4  0500a0e1  cpy r0, r5
108ae1e8  0d78feeb  bl 0x1084c224
108ae1ec  040050e3  cmp r0, #0x4
108ae1f0  0060a0e1  cpy r6, r0
108ae1f4  2000000a  beq 0x108ae27c
108ae1f8  0010a0e1  cpy r1, r0
108ae1fc  0500a0e1  cpy r0, r5
108ae200  b78ffeeb  bl 0x108520e4
108ae204  ff5000e2  and r5, r0, #0xff
108ae208  040055e3  cmp r5, #0x4
108ae20c  0450a083  movhi r5, #0x4
108ae210  00005593  cmpls r5, #0x0
108ae214  1800000a  beq 0x108ae27c
108ae218  0420a0e1  cpy r2, r4
108ae21c  0910a0e3  mov r1, #0x9
108ae220  0800a0e1  cpy r0, r8
108ae224  54effeeb  bl 0x10869f7c
108ae228  0660c0e5  strb r6, [r0,#0x6]
108ae22c  0570c0e5  strb r7, [r0,#0x5]
108ae230  0040a0e1  cpy r4, r0
108ae234  0450c0e5  strb r5, [r0,#0x4]
108ae238  40209fe5  ldr r2, [0x108ae280]
108ae23c  0210a0e3  mov r1, #0x2
108ae240  080080e2  add r0, r0, #0x8
108ae244  227cfeeb  bl 0x1084d2d4
108ae248  0710a0e1  cpy r1, r7
108ae24c  080084e2  add r0, r4, #0x8
108ae250  117cfeeb  bl 0x1084d29c
108ae254  0910a0e1  cpy r1, r9
108ae258  080084e2  add r0, r4, #0x8
108ae25c  0e7cfeeb  bl 0x1084d29c
108ae260  0510a0e1  cpy r1, r5
108ae264  080084e2  add r0, r4, #0x8
108ae268  0b7cfeeb  bl 0x1084d29c
108ae26c  0410a0e1  cpy r1, r4
108ae270  0800a0e1  cpy r0, r8
108ae274  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108ae278  32e6feea  b 0x10867b48
108ae27c  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
