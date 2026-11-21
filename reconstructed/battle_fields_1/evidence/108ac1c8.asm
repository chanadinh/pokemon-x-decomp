108ac1c8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ac1cc  0160a0e1  cpy r6, r1
108ac1d0  0240a0e1  cpy r4, r2
108ac1d4  0200a0e3  mov r0, #0x2
108ac1d8  cffdfeeb  bl 0x1086b91c
108ac1dc  ff5000e2  and r5, r0, #0xff
108ac1e0  1f0055e3  cmp r5, #0x1f
108ac1e4  3900000a  beq 0x108ac2d0
108ac1e8  0510a0e1  cpy r1, r5
108ac1ec  0600a0e1  cpy r0, r6
108ac1f0  6a7ffeeb  bl 0x1084bfa0
108ac1f4  000050e3  cmp r0, #0x0
108ac1f8  3400000a  beq 0x108ac2d0
108ac1fc  0600a0e1  cpy r0, r6
108ac200  90e6feeb  bl 0x10865c48
108ac204  000050e3  cmp r0, #0x0
108ac208  3000001a  bne 0x108ac2d0
108ac20c  0500a0e1  cpy r0, r5
108ac210  6a7ffeeb  bl 0x1084bfc0
108ac214  040050e1  cmp r0, r4
108ac218  2c00001a  bne 0x108ac2d0
108ac21c  0510a0e1  cpy r1, r5
108ac220  0600a0e1  cpy r0, r6
108ac224  9da0feeb  bl 0x108544a0
108ac228  0070a0e1  cpy r7, r0
108ac22c  0910a0e3  mov r1, #0x9
108ac230  ca0dffeb  bl 0x1086f960
108ac234  000050e3  cmp r0, #0x0
108ac238  2400001a  bne 0x108ac2d0
108ac23c  0520a0e1  cpy r2, r5
108ac240  3810a0e3  mov r1, #0x38
108ac244  0600a0e1  cpy r0, r6
108ac248  4bf7feeb  bl 0x10869f7c
108ac24c  0040a0e1  cpy r4, r0
108ac250  0510a0e1  cpy r1, r5
108ac254  0600a0e1  cpy r0, r6
108ac258  be1bffeb  bl 0x10873158
108ac25c  0600c4e5  strb r0, [r4,#0x6]
108ac260  0a20a0e3  mov r2, #0xa
108ac264  9f1fa0e3  mov r1, #0x27c
108ac268  0720c4e5  strb r2, [r4,#0x7]
108ac26c  b410c4e1  strh r1, [r4,#0x4]
108ac270  0410a0e1  cpy r1, r4
108ac274  0600a0e1  cpy r0, r6
108ac278  32eefeeb  bl 0x10867b48
108ac27c  1f20a0e3  mov r2, #0x1f
108ac280  0610a0e3  mov r1, #0x6
108ac284  0600a0e1  cpy r0, r6
108ac288  3bf7feeb  bl 0x10869f7c
108ac28c  0040a0e1  cpy r4, r0
108ac290  0650c0e5  strb r5, [r0,#0x6]
108ac294  0810a0e3  mov r1, #0x8
108ac298  0700a0e1  cpy r0, r7
108ac29c  1484feeb  bl 0x1084d2f4
108ac2a0  b400c4e1  strh r0, [r4,#0x4]
108ac2a4  28209fe5  ldr r2, [0x108ac2d4]
108ac2a8  0210a0e3  mov r1, #0x2
108ac2ac  0c0084e2  add r0, r4, #0xc
108ac2b0  0784feeb  bl 0x1084d2d4
108ac2b4  0510a0e1  cpy r1, r5
108ac2b8  0c0084e2  add r0, r4, #0xc
108ac2bc  f683feeb  bl 0x1084d29c
108ac2c0  0410a0e1  cpy r1, r4
108ac2c4  0600a0e1  cpy r0, r6
108ac2c8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ac2cc  1deefeea  b 0x10867b48
108ac2d0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
