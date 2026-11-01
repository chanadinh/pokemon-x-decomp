108bae4c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bae50  0260a0e1  cpy r6, r2
108bae54  0170a0e1  cpy r7, r1
108bae58  180093e5  ldr r0, [r3,#0x18]
108bae5c  000050e3  cmp r0, #0x0
108bae60  1d00000a  beq 0x108baedc
108bae64  1e00a0e3  mov r0, #0x1e
108bae68  abc2feeb  bl 0x1086b91c
108bae6c  020050e3  cmp r0, #0x2
108bae70  1900001a  bne 0x108baedc
108bae74  0110a0e3  mov r1, #0x1
108bae78  4200a0e3  mov r0, #0x42
108bae7c  94aafeeb  bl 0x108658d4
108bae80  000050e3  cmp r0, #0x0
108bae84  1400000a  beq 0x108baedc
108bae88  0620a0e1  cpy r2, r6
108bae8c  0310a0e3  mov r1, #0x3
108bae90  0700a0e1  cpy r0, r7
108bae94  38bcfeeb  bl 0x10869f7c
108bae98  0040a0e1  cpy r4, r0
108bae9c  0400a0e3  mov r0, #0x4
108baea0  9dc2feeb  bl 0x1086b91c
108baea4  ff5000e2  and r5, r0, #0xff
108baea8  060055e1  cmp r5, r6
108baeac  2c209f15  ldrne r2, [0x108baee0]
108baeb0  b22fa003  moveq r2, #0x2c8
108baeb4  0210a0e3  mov r1, #0x2
108baeb8  040084e2  add r0, r4, #0x4
108baebc  0449feeb  bl 0x1084d2d4
108baec0  0510a0e1  cpy r1, r5
108baec4  040084e2  add r0, r4, #0x4
108baec8  f348feeb  bl 0x1084d29c
108baecc  0410a0e1  cpy r1, r4
108baed0  0700a0e1  cpy r0, r7
108baed4  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108baed8  1ab3feea  b 0x10867b48
108baedc  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
