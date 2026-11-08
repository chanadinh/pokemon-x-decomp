108bff30  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bff34  08d04de2  sub sp, sp, #0x8
108bff38  0170a0e1  cpy r7, r1
108bff3c  0250a0e1  cpy r5, r2
108bff40  0200a0e3  mov r0, #0x2
108bff44  74aefeeb  bl 0x1086b91c
108bff48  050050e1  cmp r0, r5
108bff4c  3400001a  bne 0x108c0024
108bff50  0510a0e1  cpy r1, r5
108bff54  0700a0e1  cpy r0, r7
108bff58  7eccfeeb  bl 0x10873158
108bff5c  011b80e3  orr r1, r0, #0x400
108bff60  0d20a0e1  cpy r2, sp
108bff64  0700a0e1  cpy r0, r7
108bff68  2c2ffeeb  bl 0x1084bc20
108bff6c  006050e2  subs r6, r0, #0x0
108bff70  0040a0e3  mov r4, #0x0
108bff74  2a00009a  bls 0x108c0024
108bff78  0d80a0e1  cpy r8, sp
108bff7c  0190a0e3  mov r9, #0x1
108bff80  26a0a0e3  mov r10, #0x26
108bff84  0410d8e7  ldrb r1, [r8,r4]
108bff88  050051e1  cmp r1, r5
108bff8c  2000000a  beq 0x108c0014
108bff90  0700a0e1  cpy r0, r7
108bff94  4151feeb  bl 0x108544a0
108bff98  00f020e3  nop
108bff9c  00f020e3  nop
108bffa0  41b6feeb  bl 0x1086d8ac
108bffa4  000050e3  cmp r0, #0x0
108bffa8  00f020e3  nop
108bffac  1800000a  beq 0x108c0014
108bffb0  1e00a0e3  mov r0, #0x1e
108bffb4  4f30feeb  bl 0x1084c0f8
108bffb8  000050e3  cmp r0, #0x0
108bffbc  00f020e3  nop
108bffc0  0500001a  bne 0x108bffdc
108bffc4  0110a0e3  mov r1, #0x1
108bffc8  0700a0e1  cpy r0, r7
108bffcc  4630feeb  bl 0x1084c0ec
108bffd0  000050e3  cmp r0, #0x0
108bffd4  00f020e3  nop
108bffd8  0d00000a  beq 0x108c0014
108bffdc  0520a0e1  cpy r2, r5
108bffe0  0a10a0e3  mov r1, #0xa
108bffe4  0700a0e1  cpy r0, r7
108bffe8  e3a7feeb  bl 0x10869f7c
108bffec  0010a0e1  cpy r1, r0
108bfff0  000090e5  ldr r0, [r0,#0x0]
108bfff4  1190c1e5  strb r9, [r1,#0x11]
108bfff8  0a0580e3  orr r0, r0, #0x2800000
108bfffc  000081e5  str r0, [r1,#0x0]
108c0000  0400d8e7  ldrb r0, [r8,r4]
108c0004  04a0c1e5  strb r10, [r1,#0x4]
108c0008  0500c1e5  strb r0, [r1,#0x5]
108c000c  0700a0e1  cpy r0, r7
108c0010  cc9efeeb  bl 0x10867b48
108c0014  010084e2  add r0, r4, #0x1
108c0018  ff4000e2  and r4, r0, #0xff
108c001c  060054e1  cmp r4, r6
108c0020  d7ffff3a  bcc 0x108bff84
108c0024  08d08de2  add sp, sp, #0x8
108c0028  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
