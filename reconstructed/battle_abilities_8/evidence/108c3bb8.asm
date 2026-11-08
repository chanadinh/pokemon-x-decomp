108c3bb8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c3bbc  0260a0e1  cpy r6, r2
108c3bc0  0170a0e1  cpy r7, r1
108c3bc4  0350a0e1  cpy r5, r3
108c3bc8  0200a0e3  mov r0, #0x2
108c3bcc  529ffeeb  bl 0x1086b91c
108c3bd0  060050e1  cmp r0, r6
108c3bd4  1600001a  bne 0x108c3c34
108c3bd8  000095e5  ldr r0, [r5,#0x0]
108c3bdc  000050e3  cmp r0, #0x0
108c3be0  1300000a  beq 0x108c3c34
108c3be4  0620a0e1  cpy r2, r6
108c3be8  0310a0e3  mov r1, #0x3
108c3bec  0700a0e1  cpy r0, r7
108c3bf0  e198feeb  bl 0x10869f7c
108c3bf4  0040a0e1  cpy r4, r0
108c3bf8  000090e5  ldr r0, [r0,#0x0]
108c3bfc  34209fe5  ldr r2, [0x108c3c38]
108c3c00  0210a0e3  mov r1, #0x2
108c3c04  020580e3  orr r0, r0, #0x800000
108c3c08  000084e5  str r0, [r4,#0x0]
108c3c0c  040084e2  add r0, r4, #0x4
108c3c10  af25feeb  bl 0x1084d2d4
108c3c14  0610a0e1  cpy r1, r6
108c3c18  040084e2  add r0, r4, #0x4
108c3c1c  9e25feeb  bl 0x1084d29c
108c3c20  0410a0e1  cpy r1, r4
108c3c24  0700a0e1  cpy r0, r7
108c3c28  c68ffeeb  bl 0x10867b48
108c3c2c  0000a0e3  mov r0, #0x0
108c3c30  000085e5  str r0, [r5,#0x0]
108c3c34  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
