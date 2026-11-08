108c39b0  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108c39b4  0150a0e1  cpy r5, r1
108c39b8  0060a0e1  cpy r6, r0
108c39bc  000093e5  ldr r0, [r3,#0x0]
108c39c0  0240a0e1  cpy r4, r2
108c39c4  000050e3  cmp r0, #0x0
108c39c8  1100000a  beq 0x108c3a14
108c39cc  0200a0e3  mov r0, #0x2
108c39d0  d19ffeeb  bl 0x1086b91c
108c39d4  040050e1  cmp r0, r4
108c39d8  0d00001a  bne 0x108c3a14
108c39dc  0500a0e1  cpy r0, r5
108c39e0  0410a0e1  cpy r1, r4
108c39e4  ad42feeb  bl 0x108544a0
108c39e8  bc00d0e1  ldrh r0, [r0,#0xc]
108c39ec  012c40e2  sub r2, r0, #0x100
108c39f0  a52052e2  subs r2, r2, #0xa5
108c39f4  0010a0e3  mov r1, #0x0
108c39f8  0500001a  bne 0x108c3a14
108c39fc  00108de5  str r1, [sp,#0x0]
108c3a00  0030a0e3  mov r3, #0x0
108c3a04  0420a0e1  cpy r2, r4
108c3a08  0510a0e1  cpy r1, r5
108c3a0c  0600a0e1  cpy r0, r6
108c3a10  cb1bfeeb  bl 0x1084a944
108c3a14  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
