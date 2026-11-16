108bba4c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bba50  0160a0e1  cpy r6, r1
108bba54  0250a0e1  cpy r5, r2
108bba58  0300a0e3  mov r0, #0x3
108bba5c  aebffeeb  bl 0x1086b91c
108bba60  050050e1  cmp r0, r5
108bba64  3600001a  bne 0x108bbb44
108bba68  0400a0e3  mov r0, #0x4
108bba6c  aabffeeb  bl 0x1086b91c
108bba70  ff0000e2  and r0, r0, #0xff
108bba74  5141feeb  bl 0x1084bfc0
108bba78  0040a0e1  cpy r4, r0
108bba7c  c4109fe5  ldr r1, [0x108bbb48]
108bba80  040280e0  add r0, r0, r4, lsl #0x4
108bba84  000281e0  add r0, r1, r0, lsl #0x4
108bba88  001090e5  ldr r1, [r0,#0x0]
108bba8c  000051e3  cmp r1, #0x0
108bba90  10009005  ldreq r0, [r0,#0x10]
108bba94  00005003  cmpeq r0, #0x0
108bba98  2900000a  beq 0x108bbb44
108bba9c  0520a0e1  cpy r2, r5
108bbaa0  1a10a0e3  mov r1, #0x1a
108bbaa4  0600a0e1  cpy r0, r6
108bbaa8  33b9feeb  bl 0x10869f7c
108bbaac  0030a0e1  cpy r3, r0
108bbab0  0410a0e3  mov r1, #0x4
108bbab4  0840c0e5  strb r4, [r0,#0x8]
108bbab8  0410e0e5  strb r1, [r0,#0x4]!
108bbabc  0010a0e3  mov r1, #0x0
108bbac0  0120a0e3  mov r2, #0x1
108bbac4  0110e0e5  strb r1, [r0,#0x1]!
108bbac8  012052e2  subs r2, r2, #0x1
108bbacc  0110c0e5  strb r1, [r0,#0x1]
108bbad0  0210e0e5  strb r1, [r0,#0x2]!
108bbad4  fbffff1a  bne 0x108bbac8
108bbad8  0410d3e5  ldrb r1, [r3,#0x4]
108bbadc  040083e2  add r0, r3, #0x4
108bbae0  0140a0e3  mov r4, #0x1
108bbae4  010051e3  cmp r1, #0x1
108bbae8  0200009a  bls 0x108bbaf8
108bbaec  0110d0e5  ldrb r1, [r0,#0x1]
108bbaf0  011081e3  orr r1, r1, #0x1
108bbaf4  0110c0e5  strb r1, [r0,#0x1]
108bbaf8  0410d3e5  ldrb r1, [r3,#0x4]
108bbafc  040083e2  add r0, r3, #0x4
108bbb00  010051e3  cmp r1, #0x1
108bbb04  0200009a  bls 0x108bbb14
108bbb08  0110d0e5  ldrb r1, [r0,#0x1]
108bbb0c  021081e3  orr r1, r1, #0x2
108bbb10  0110c0e5  strb r1, [r0,#0x1]
108bbb14  0310a0e1  cpy r1, r3
108bbb18  0600a0e1  cpy r0, r6
108bbb1c  09b0feeb  bl 0x10867b48
108bbb20  0520a0e1  cpy r2, r5
108bbb24  3b10a0e3  mov r1, #0x3b
108bbb28  0600a0e1  cpy r0, r6
108bbb2c  12b9feeb  bl 0x10869f7c
108bbb30  0010a0e1  cpy r1, r0
108bbb34  0440c0e5  strb r4, [r0,#0x4]
108bbb38  0600a0e1  cpy r0, r6
108bbb3c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bbb40  00b0feea  b 0x10867b48
108bbb44  7080bde8  ldmia sp!,{r4,r5,r6,pc}
