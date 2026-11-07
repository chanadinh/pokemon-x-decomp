108be5f8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108be5fc  0160a0e1  cpy r6, r1
108be600  0250a0e1  cpy r5, r2
108be604  0400a0e3  mov r0, #0x4
108be608  c3b4feeb  bl 0x1086b91c
108be60c  050050e1  cmp r0, r5
108be610  2800001a  bne 0x108be6b8
108be614  4700a0e3  mov r0, #0x47
108be618  bfb4feeb  bl 0x1086b91c
108be61c  000050e3  cmp r0, #0x0
108be620  2400001a  bne 0x108be6b8
108be624  1200a0e3  mov r0, #0x12
108be628  bbb4feeb  bl 0x1086b91c
108be62c  7000ffe6  uxth r0, r0
108be630  0010a0e3  mov r1, #0x0
108be634  9807fdeb  bl 0x1080049c
108be638  000050e3  cmp r0, #0x0
108be63c  1d00000a  beq 0x108be6b8
108be640  0300a0e3  mov r0, #0x3
108be644  b4b4feeb  bl 0x1086b91c
108be648  ff4000e2  and r4, r0, #0xff
108be64c  0410a0e1  cpy r1, r4
108be650  0600a0e1  cpy r0, r6
108be654  9157feeb  bl 0x108544a0
108be658  7984feeb  bl 0x1085f844
108be65c  000050e3  cmp r0, #0x0
108be660  1400001a  bne 0x108be6b8
108be664  0520a0e1  cpy r2, r5
108be668  0d10a0e3  mov r1, #0xd
108be66c  0600a0e1  cpy r0, r6
108be670  41aefeeb  bl 0x10869f7c
108be674  1820d0e5  ldrb r2, [r0,#0x18]
108be678  003090e5  ldr r3, [r0,#0x0]
108be67c  0010a0e1  cpy r1, r0
108be680  0150a0e3  mov r5, #0x1
108be684  05c0a0e3  mov r12, #0x5
108be688  1650c1e5  strb r5, [r1,#0x16]
108be68c  023583e3  orr r3, r3, #0x800000
108be690  04c0c1e5  strb r12, [r1,#0x4]
108be694  ff00a0e3  mov r0, #0xff
108be698  003081e5  str r3, [r1,#0x0]
108be69c  1700c1e5  strb r0, [r1,#0x17]
108be6a0  122082e3  orr r2, r2, #0x12
108be6a4  1820c1e5  strb r2, [r1,#0x18]
108be6a8  0600a0e1  cpy r0, r6
108be6ac  0c40c1e5  strb r4, [r1,#0xc]
108be6b0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108be6b4  23a5feea  b 0x10867b48
108be6b8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
