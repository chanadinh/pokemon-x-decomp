108bc9a8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bc9ac  0260a0e1  cpy r6, r2
108bc9b0  0170a0e1  cpy r7, r1
108bc9b4  0400a0e3  mov r0, #0x4
108bc9b8  d7bbfeeb  bl 0x1086b91c
108bc9bc  060050e1  cmp r0, r6
108bc9c0  3300001a  bne 0x108bca94
108bc9c4  4700a0e3  mov r0, #0x47
108bc9c8  d3bbfeeb  bl 0x1086b91c
108bc9cc  000050e3  cmp r0, #0x0
108bc9d0  2f00001a  bne 0x108bca94
108bc9d4  0700a0e1  cpy r0, r7
108bc9d8  9aa4feeb  bl 0x10865c48
108bc9dc  000050e3  cmp r0, #0x0
108bc9e0  2b00001a  bne 0x108bca94
108bc9e4  1200a0e3  mov r0, #0x12
108bc9e8  cbbbfeeb  bl 0x1086b91c
108bc9ec  7000ffe6  uxth r0, r0
108bc9f0  0010a0e3  mov r1, #0x0
108bc9f4  a80efdeb  bl 0x1080049c
108bc9f8  000050e3  cmp r0, #0x0
108bc9fc  2400000a  beq 0x108bca94
108bca00  0300a0e3  mov r0, #0x3
108bca04  c4bbfeeb  bl 0x1086b91c
108bca08  ff5000e2  and r5, r0, #0xff
108bca0c  0510a0e1  cpy r1, r5
108bca10  0700a0e1  cpy r0, r7
108bca14  a15efeeb  bl 0x108544a0
108bca18  1010a0e3  mov r1, #0x10
108bca1c  7fdefeeb  bl 0x10874420
108bca20  980050e3  cmp r0, #0x98
108bca24  1a00000a  beq 0x108bca94
108bca28  0620a0e1  cpy r2, r6
108bca2c  2010a0e3  mov r1, #0x20
108bca30  0700a0e1  cpy r0, r7
108bca34  50b5feeb  bl 0x10869f7c
108bca38  0040a0e1  cpy r4, r0
108bca3c  9800a0e3  mov r0, #0x98
108bca40  0650c4e5  strb r5, [r4,#0x6]
108bca44  b400c4e1  strh r0, [r4,#0x4]
108bca48  48209fe5  ldr r2, [0x108bca98]
108bca4c  0210a0e3  mov r1, #0x2
108bca50  0c0084e2  add r0, r4, #0xc
108bca54  1e42feeb  bl 0x1084d2d4
108bca58  0610d4e5  ldrb r1, [r4,#0x6]
108bca5c  0c0084e2  add r0, r4, #0xc
108bca60  0d42feeb  bl 0x1084d29c
108bca64  0510a0e1  cpy r1, r5
108bca68  0600a0e1  cpy r0, r6
108bca6c  cd3efeeb  bl 0x1084c5a8
108bca70  000050e3  cmp r0, #0x0
108bca74  0200001a  bne 0x108bca84
108bca78  000094e5  ldr r0, [r4,#0x0]
108bca7c  020580e3  orr r0, r0, #0x800000
108bca80  000084e5  str r0, [r4,#0x0]
108bca84  0410a0e1  cpy r1, r4
108bca88  0700a0e1  cpy r0, r7
108bca8c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bca90  2cacfeea  b 0x10867b48
108bca94  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
