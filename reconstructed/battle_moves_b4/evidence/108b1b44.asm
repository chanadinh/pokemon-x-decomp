108b1b44  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b1b48  0240a0e1  cpy r4, r2
108b1b4c  0170a0e1  cpy r7, r1
108b1b50  0300a0e3  mov r0, #0x3
108b1b54  70e7feeb  bl 0x1086b91c
108b1b58  040050e1  cmp r0, r4
108b1b5c  4500001a  bne 0x108b1c78
108b1b60  0410a0e1  cpy r1, r4
108b1b64  0700a0e1  cpy r0, r7
108b1b68  4c8afeeb  bl 0x108544a0
108b1b6c  0080a0e1  cpy r8, r0
108b1b70  0210a0e3  mov r1, #0x2
108b1b74  de6dfeeb  bl 0x1084d2f4
108b1b78  0060a0e1  cpy r6, r0
108b1b7c  0110a0e3  mov r1, #0x1
108b1b80  0800a0e1  cpy r0, r8
108b1b84  2867feeb  bl 0x1084b82c
108b1b88  0050a0e1  cpy r5, r0
108b1b8c  0d10a0e3  mov r1, #0xd
108b1b90  0800a0e1  cpy r0, r8
108b1b94  210affeb  bl 0x10874420
108b1b98  060050e1  cmp r0, r6
108b1b9c  3500009a  bls 0x108b1c78
108b1ba0  000055e3  cmp r5, #0x0
108b1ba4  3300000a  beq 0x108b1c78
108b1ba8  0420a0e1  cpy r2, r4
108b1bac  0710a0e3  mov r1, #0x7
108b1bb0  0700a0e1  cpy r0, r7
108b1bb4  f0e0feeb  bl 0x10869f7c
108b1bb8  0010a0e1  cpy r1, r0
108b1bbc  0180a0e3  mov r8, #0x1
108b1bc0  000066e2  rsb r0, r6, #0x0
108b1bc4  0480c1e5  strb r8, [r1,#0x4]
108b1bc8  140081e5  str r0, [r1,#0x14]
108b1bcc  0680c1e5  strb r8, [r1,#0x6]
108b1bd0  0700a0e1  cpy r0, r7
108b1bd4  0740c1e5  strb r4, [r1,#0x7]
108b1bd8  dad7feeb  bl 0x10867b48
108b1bdc  0420a0e1  cpy r2, r4
108b1be0  0d10a0e3  mov r1, #0xd
108b1be4  0700a0e1  cpy r0, r7
108b1be8  e3e0feeb  bl 0x10869f7c
108b1bec  0010a0e1  cpy r1, r0
108b1bf0  1800d0e5  ldrb r0, [r0,#0x18]
108b1bf4  0480c1e5  strb r8, [r1,#0x4]
108b1bf8  0c40c1e5  strb r4, [r1,#0xc]
108b1bfc  1750c1e5  strb r5, [r1,#0x17]
108b1c00  070080e3  orr r0, r0, #0x7
108b1c04  1680c1e5  strb r8, [r1,#0x16]
108b1c08  1800c1e5  strb r0, [r1,#0x18]
108b1c0c  0700a0e1  cpy r0, r7
108b1c10  ccd7feeb  bl 0x10867b48
108b1c14  0420a0e1  cpy r2, r4
108b1c18  0310a0e3  mov r1, #0x3
108b1c1c  0700a0e1  cpy r0, r7
108b1c20  d5e0feeb  bl 0x10869f7c
108b1c24  0050a0e1  cpy r5, r0
108b1c28  992f88e2  add r2, r8, #0x264
108b1c2c  0210a0e3  mov r1, #0x2
108b1c30  040080e2  add r0, r0, #0x4
108b1c34  a66dfeeb  bl 0x1084d2d4
108b1c38  0410a0e1  cpy r1, r4
108b1c3c  040085e2  add r0, r5, #0x4
108b1c40  956dfeeb  bl 0x1084d29c
108b1c44  0510a0e1  cpy r1, r5
108b1c48  0700a0e1  cpy r0, r7
108b1c4c  bdd7feeb  bl 0x10867b48
108b1c50  0420a0e1  cpy r2, r4
108b1c54  2210a0e3  mov r1, #0x22
108b1c58  0700a0e1  cpy r0, r7
108b1c5c  c6e0feeb  bl 0x10869f7c
108b1c60  0440c0e5  strb r4, [r0,#0x4]
108b1c64  0010a0e1  cpy r1, r0
108b1c68  0580c0e5  strb r8, [r0,#0x5]
108b1c6c  0700a0e1  cpy r0, r7
108b1c70  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108b1c74  b3d7feea  b 0x10867b48
108b1c78  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
