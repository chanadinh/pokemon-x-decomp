108ac300  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ac304  0250a0e1  cpy r5, r2
108ac308  0070a0e1  cpy r7, r0
108ac30c  0180a0e1  cpy r8, r1
108ac310  0200a0e3  mov r0, #0x2
108ac314  80fdfeeb  bl 0x1086b91c
108ac318  ff6000e2  and r6, r0, #0xff
108ac31c  0600a0e1  cpy r0, r6
108ac320  267ffeeb  bl 0x1084bfc0
108ac324  050050e1  cmp r0, r5
108ac328  4a00001a  bne 0x108ac458
108ac32c  0610a0e1  cpy r1, r6
108ac330  0800a0e1  cpy r0, r8
108ac334  fa7efeeb  bl 0x1084bf24
108ac338  000050e3  cmp r0, #0x0
108ac33c  4500001a  bne 0x108ac458
108ac340  0610a0e1  cpy r1, r6
108ac344  0800a0e1  cpy r0, r8
108ac348  54a0feeb  bl 0x108544a0
108ac34c  0310a0e3  mov r1, #0x3
108ac350  820dffeb  bl 0x1086f960
108ac354  000050e3  cmp r0, #0x0
108ac358  0040a0e3  mov r4, #0x0
108ac35c  1800000a  beq 0x108ac3c4
108ac360  0620a0e1  cpy r2, r6
108ac364  1a10a0e3  mov r1, #0x1a
108ac368  0800a0e1  cpy r0, r8
108ac36c  02f7feeb  bl 0x10869f7c
108ac370  0030a0e1  cpy r3, r0
108ac374  0410a0e3  mov r1, #0x4
108ac378  0850c0e5  strb r5, [r0,#0x8]
108ac37c  0410e0e5  strb r1, [r0,#0x4]!
108ac380  0110a0e3  mov r1, #0x1
108ac384  0140e0e5  strb r4, [r0,#0x1]!
108ac388  011051e2  subs r1, r1, #0x1
108ac38c  0140c0e5  strb r4, [r0,#0x1]
108ac390  0240e0e5  strb r4, [r0,#0x2]!
108ac394  fbffff1a  bne 0x108ac388
108ac398  0400d3e5  ldrb r0, [r3,#0x4]
108ac39c  042083e2  add r2, r3, #0x4
108ac3a0  010050e3  cmp r0, #0x1
108ac3a4  0200009a  bls 0x108ac3b4
108ac3a8  0100d2e5  ldrb r0, [r2,#0x1]
108ac3ac  800080e3  orr r0, r0, #0x80
108ac3b0  0100c2e5  strb r0, [r2,#0x1]
108ac3b4  0800a0e1  cpy r0, r8
108ac3b8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ac3bc  0310a0e1  cpy r1, r3
108ac3c0  e0edfeea  b 0x10867b48
108ac3c4  1f20a0e3  mov r2, #0x1f
108ac3c8  0b10a0e3  mov r1, #0xb
108ac3cc  0800a0e1  cpy r0, r8
108ac3d0  e9f6feeb  bl 0x10869f7c
108ac3d4  0040a0e1  cpy r4, r0
108ac3d8  0500a0e3  mov r0, #0x5
108ac3dc  b400c4e1  strh r0, [r4,#0x4]
108ac3e0  0700a0e1  cpy r0, r7
108ac3e4  b803d0e1  ldrh r0, [r0,#0x38]
108ac3e8  ff0000e2  and r0, r0, #0xff
108ac3ec  110050e3  cmp r0, #0x11
108ac3f0  02005533  cmpcc r5, #0x2
108ac3f4  0b00002a  bcs 0x108ac428
108ac3f8  5c209fe5  ldr r2, [0x108ac45c]
108ac3fc  051285e0  add r1, r5, r5, lsl #0x4
108ac400  011282e0  add r1, r2, r1, lsl #0x4
108ac404  000281e0  add r0, r1, r0, lsl #0x4
108ac408  0c0090e5  ldr r0, [r0,#0xc]
108ac40c  ff0000e2  and r0, r0, #0xff
108ac410  010050e3  cmp r0, #0x1
108ac414  0700009a  bls 0x108ac438
108ac418  7900a0e3  mov r0, #0x79
108ac41c  00f020e3  nop
108ac420  00f020e3  nop
108ac424  050000ea  b 0x108ac440
108ac428  0020a0e3  mov r2, #0x0
108ac42c  0210a0e1  cpy r1, r2
108ac430  0200a0e1  cpy r0, r2
108ac434  d64ffdeb  bl 0x10800394
108ac438  00f020e3  nop
108ac43c  0100a0e3  mov r0, #0x1
108ac440  080084e5  str r0, [r4,#0x8]
108ac444  0410a0e1  cpy r1, r4
108ac448  0800a0e1  cpy r0, r8
108ac44c  0f60c4e5  strb r6, [r4,#0xf]
108ac450  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ac454  bbedfeea  b 0x10867b48
108ac458  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
