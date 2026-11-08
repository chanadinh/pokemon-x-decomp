108bda2c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bda30  0160a0e1  cpy r6, r1
108bda34  0250a0e1  cpy r5, r2
108bda38  0400a0e3  mov r0, #0x4
108bda3c  b6b7feeb  bl 0x1086b91c
108bda40  050050e1  cmp r0, r5
108bda44  1700001a  bne 0x108bdaa8
108bda48  0110a0e3  mov r1, #0x1
108bda4c  4200a0e3  mov r0, #0x42
108bda50  9f9ffeeb  bl 0x108658d4
108bda54  000050e3  cmp r0, #0x0
108bda58  1200000a  beq 0x108bdaa8
108bda5c  0520a0e1  cpy r2, r5
108bda60  0310a0e3  mov r1, #0x3
108bda64  0600a0e1  cpy r0, r6
108bda68  43b1feeb  bl 0x10869f7c
108bda6c  0040a0e1  cpy r4, r0
108bda70  000090e5  ldr r0, [r0,#0x0]
108bda74  30209fe5  ldr r2, [0x108bdaac]
108bda78  0210a0e3  mov r1, #0x2
108bda7c  020580e3  orr r0, r0, #0x800000
108bda80  000084e5  str r0, [r4,#0x0]
108bda84  040084e2  add r0, r4, #0x4
108bda88  113efeeb  bl 0x1084d2d4
108bda8c  0510a0e1  cpy r1, r5
108bda90  040084e2  add r0, r4, #0x4
108bda94  003efeeb  bl 0x1084d29c
108bda98  0410a0e1  cpy r1, r4
108bda9c  0600a0e1  cpy r0, r6
108bdaa0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bdaa4  27a8feea  b 0x10867b48
108bdaa8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
