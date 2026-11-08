108c4bac  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108c4bb0  0340a0e1  cpy r4, r3
108c4bb4  0170a0e1  cpy r7, r1
108c4bb8  0260a0e1  cpy r6, r2
108c4bbc  0200a0e3  mov r0, #0x2
108c4bc0  559bfeeb  bl 0x1086b91c
108c4bc4  060050e1  cmp r0, r6
108c4bc8  08009405  ldreq r0, [r4,#0x8]
108c4bcc  00005003  cmpeq r0, #0x0
108c4bd0  2300001a  bne 0x108c4c64
108c4bd4  040094e5  ldr r0, [r4,#0x4]
108c4bd8  050050e3  cmp r0, #0x5
108c4bdc  0e00002a  bcs 0x108c4c1c
108c4be0  0d20a0e1  cpy r2, sp
108c4be4  0610a0e1  cpy r1, r6
108c4be8  0700a0e1  cpy r0, r7
108c4bec  e41afeeb  bl 0x1084b784
108c4bf0  000050e3  cmp r0, #0x0
108c4bf4  1a00000a  beq 0x108c4c64
108c4bf8  00009de5  ldr r0, [sp,#0x0]
108c4bfc  0f0000e2  and r0, r0, #0xf
108c4c00  030050e3  cmp r0, #0x3
108c4c04  1600000a  beq 0x108c4c64
108c4c08  040094e5  ldr r0, [r4,#0x4]
108c4c0c  010080e2  add r0, r0, #0x1
108c4c10  050050e3  cmp r0, #0x5
108c4c14  040084e5  str r0, [r4,#0x4]
108c4c18  1100003a  bcc 0x108c4c64
108c4c1c  0620a0e1  cpy r2, r6
108c4c20  0310a0e3  mov r1, #0x3
108c4c24  0700a0e1  cpy r0, r7
108c4c28  d394feeb  bl 0x10869f7c
108c4c2c  0050a0e1  cpy r5, r0
108c4c30  30209fe5  ldr r2, [0x108c4c68]
108c4c34  0210a0e3  mov r1, #0x2
108c4c38  040080e2  add r0, r0, #0x4
108c4c3c  a421feeb  bl 0x1084d2d4
108c4c40  0610a0e1  cpy r1, r6
108c4c44  040085e2  add r0, r5, #0x4
108c4c48  9321feeb  bl 0x1084d29c
108c4c4c  0510a0e1  cpy r1, r5
108c4c50  0700a0e1  cpy r0, r7
108c4c54  bb8bfeeb  bl 0x10867b48
108c4c58  0010a0e3  mov r1, #0x0
108c4c5c  0100a0e3  mov r0, #0x1
108c4c60  f800c4e1  strd r0, r1, [r4,#0x8]
108c4c64  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
