108afd74  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108afd78  0250a0e1  cpy r5, r2
108afd7c  0180a0e1  cpy r8, r1
108afd80  0300a0e3  mov r0, #0x3
108afd84  e4eefeeb  bl 0x1086b91c
108afd88  050050e1  cmp r0, r5
108afd8c  2e00001a  bne 0x108afe4c
108afd90  0600a0e3  mov r0, #0x6
108afd94  e0eefeeb  bl 0x1086b91c
108afd98  ff6000e2  and r6, r0, #0xff
108afd9c  0610a0e1  cpy r1, r6
108afda0  0800a0e1  cpy r0, r8
108afda4  bd91feeb  bl 0x108544a0
108afda8  b201d0e1  ldrh r0, [r0,#0x12]
108afdac  0070a0e1  cpy r7, r0
108afdb0  7b42fdeb  bl 0x108007a4
108afdb4  000050e3  cmp r0, #0x0
108afdb8  2300000a  beq 0x108afe4c
108afdbc  0520a0e1  cpy r2, r5
108afdc0  2110a0e3  mov r1, #0x21
108afdc4  0800a0e1  cpy r0, r8
108afdc8  6be8feeb  bl 0x10869f7c
108afdcc  0040a0e1  cpy r4, r0
108afdd0  0000a0e3  mov r0, #0x0
108afdd4  0660c4e5  strb r6, [r4,#0x6]
108afdd8  b400c4e1  strh r0, [r4,#0x4]
108afddc  c22fa0e3  mov r2, #0x308
108afde0  0210a0e3  mov r1, #0x2
108afde4  0c0084e2  add r0, r4, #0xc
108afde8  3975feeb  bl 0x1084d2d4
108afdec  0510a0e1  cpy r1, r5
108afdf0  0c0084e2  add r0, r4, #0xc
108afdf4  2875feeb  bl 0x1084d29c
108afdf8  0710a0e1  cpy r1, r7
108afdfc  0c0084e2  add r0, r4, #0xc
108afe00  2575feeb  bl 0x1084d29c
108afe04  0410a0e1  cpy r1, r4
108afe08  0800a0e1  cpy r0, r8
108afe0c  4ddffeeb  bl 0x10867b48
108afe10  0520a0e1  cpy r2, r5
108afe14  2310a0e3  mov r1, #0x23
108afe18  0800a0e1  cpy r0, r8
108afe1c  56e8feeb  bl 0x10869f7c
108afe20  0010a0e1  cpy r1, r0
108afe24  000090e5  ldr r0, [r0,#0x0]
108afe28  b670c1e1  strh r7, [r1,#0x6]
108afe2c  0120a0e3  mov r2, #0x1
108afe30  0450c1e5  strb r5, [r1,#0x4]
108afe34  010480e3  orr r0, r0, #0x1000000
108afe38  0520c1e5  strb r2, [r1,#0x5]
108afe3c  000081e5  str r0, [r1,#0x0]
108afe40  0800a0e1  cpy r0, r8
108afe44  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108afe48  3edffeea  b 0x10867b48
108afe4c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
