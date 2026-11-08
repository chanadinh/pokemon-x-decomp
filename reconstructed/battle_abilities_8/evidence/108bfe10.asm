108bfe10  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bfe14  0100a0e1  cpy r0, r1
108bfe18  0180a0e1  cpy r8, r1
108bfe1c  0270a0e1  cpy r7, r2
108bfe20  8897feeb  bl 0x10865c48
108bfe24  000050e3  cmp r0, #0x0
108bfe28  2900001a  bne 0x108bfed4
108bfe2c  0050a0e3  mov r5, #0x0
108bfe30  0300a0e3  mov r0, #0x3
108bfe34  b8aefeeb  bl 0x1086b91c
108bfe38  070050e1  cmp r0, r7
108bfe3c  2400001a  bne 0x108bfed4
108bfe40  0500a0e3  mov r0, #0x5
108bfe44  b4aefeeb  bl 0x1086b91c
108bfe48  006050e2  subs r6, r0, #0x0
108bfe4c  0040a0e3  mov r4, #0x0
108bfe50  1f00009a  bls 0x108bfed4
108bfe54  060084e2  add r0, r4, #0x6
108bfe58  ff0000e2  and r0, r0, #0xff
108bfe5c  aeaefeeb  bl 0x1086b91c
108bfe60  ff1000e2  and r1, r0, #0xff
108bfe64  0800a0e1  cpy r0, r8
108bfe68  8c51feeb  bl 0x108544a0
108bfe6c  00f020e3  nop
108bfe70  00f020e3  nop
108bfe74  727efeeb  bl 0x1085f844
108bfe78  000050e3  cmp r0, #0x0
108bfe7c  01508512  addne r5, r5, #0x1
108bfe80  014084e2  add r4, r4, #0x1
108bfe84  040056e1  cmp r6, r4
108bfe88  f1ffff8a  bhi 0x108bfe54
108bfe8c  000055e3  cmp r5, #0x0
108bfe90  0f00000a  beq 0x108bfed4
108bfe94  0720a0e1  cpy r2, r7
108bfe98  0d10a0e3  mov r1, #0xd
108bfe9c  0800a0e1  cpy r0, r8
108bfea0  35a8feeb  bl 0x10869f7c
108bfea4  0010a0e1  cpy r1, r0
108bfea8  000090e5  ldr r0, [r0,#0x0]
108bfeac  0120a0e3  mov r2, #0x1
108bfeb0  0c70c1e5  strb r7, [r1,#0xc]
108bfeb4  020580e3  orr r0, r0, #0x800000
108bfeb8  1620c1e5  strb r2, [r1,#0x16]
108bfebc  000081e5  str r0, [r1,#0x0]
108bfec0  0420c1e5  strb r2, [r1,#0x4]
108bfec4  0800a0e1  cpy r0, r8
108bfec8  1750c1e5  strb r5, [r1,#0x17]
108bfecc  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bfed0  1c9ffeea  b 0x10867b48
108bfed4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
