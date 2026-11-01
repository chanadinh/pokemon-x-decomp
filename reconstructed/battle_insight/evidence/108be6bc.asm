108be6bc  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108be6c0  0200a0e3  mov r0, #0x2
108be6c4  01a0a0e1  cpy r10, r1
108be6c8  0290a0e1  cpy r9, r2
108be6cc  92b4feeb  bl 0x1086b91c
108be6d0  090050e1  cmp r0, r9
108be6d4  5900001a  bne 0x108be840
108be6d8  1410a0e3  mov r1, #0x14
108be6dc  0a00a0e1  cpy r0, r10
108be6e0  3a38feeb  bl 0x1084c7d0
108be6e4  0040a0e1  cpy r4, r0
108be6e8  0a2080e2  add r2, r0, #0xa
108be6ec  0910a0e1  cpy r1, r9
108be6f0  0a00a0e1  cpy r0, r10
108be6f4  4831feeb  bl 0x1084ac1c
108be6f8  0080a0e3  mov r8, #0x0
108be6fc  006050e2  subs r6, r0, #0x0
108be700  0850a0e1  cpy r5, r8
108be704  4d00009a  bls 0x108be840
108be708  050084e0  add r0, r4, r5
108be70c  0a10d0e5  ldrb r1, [r0,#0xa]
108be710  0a00a0e1  cpy r0, r10
108be714  6157feeb  bl 0x108544a0
108be718  00f020e3  nop
108be71c  00f020e3  nop
108be720  b201d0e1  ldrh r0, [r0,#0x12]
108be724  851084e0  add r1, r4, r5, lsl #0x1
108be728  000050e3  cmp r0, #0x0
108be72c  b000c1e1  strh r0, [r1,#0x0]
108be730  0300000a  beq 0x108be744
108be734  080084e0  add r0, r4, r8
108be738  011088e2  add r1, r8, #0x1
108be73c  ff8001e2  and r8, r1, #0xff
108be740  0f50c0e5  strb r5, [r0,#0xf]
108be744  015085e2  add r5, r5, #0x1
108be748  050056e1  cmp r6, r5
108be74c  edffff8a  bhi 0x108be708
108be750  000058e3  cmp r8, #0x0
108be754  3900000a  beq 0x108be840
108be758  0920a0e1  cpy r2, r9
108be75c  0110a0e3  mov r1, #0x1
108be760  0a00a0e1  cpy r0, r10
108be764  7738feeb  bl 0x1084c948
108be768  000058e3  cmp r8, #0x0
108be76c  d0b09f85  ldrhi r11, [0x108be844]
108be770  0050a0e3  mov r5, #0x0
108be774  1a00009a  bls 0x108be7e4
108be778  0920a0e1  cpy r2, r9
108be77c  0310a0e3  mov r1, #0x3
108be780  0a00a0e1  cpy r0, r10
108be784  fcadfeeb  bl 0x10869f7c
108be788  0060a0e1  cpy r6, r0
108be78c  050084e0  add r0, r4, r5
108be790  000055e3  cmp r5, #0x0
108be794  0f70d0e5  ldrb r7, [r0,#0xf]
108be798  1600000a  beq 0x108be7f8
108be79c  fe20a0e3  mov r2, #0xfe
108be7a0  0110a0e3  mov r1, #0x1
108be7a4  040086e2  add r0, r6, #0x4
108be7a8  c93afeeb  bl 0x1084d2d4
108be7ac  070084e0  add r0, r4, r7
108be7b0  0a10d0e5  ldrb r1, [r0,#0xa]
108be7b4  040086e2  add r0, r6, #0x4
108be7b8  b73afeeb  bl 0x1084d29c
108be7bc  870084e0  add r0, r4, r7, lsl #0x1
108be7c0  b010d0e1  ldrh r1, [r0,#0x0]
108be7c4  040086e2  add r0, r6, #0x4
108be7c8  b33afeeb  bl 0x1084d29c
108be7cc  0610a0e1  cpy r1, r6
108be7d0  0a00a0e1  cpy r0, r10
108be7d4  dba4feeb  bl 0x10867b48
108be7d8  015085e2  add r5, r5, #0x1
108be7dc  080055e1  cmp r5, r8
108be7e0  e4ffff3a  bcc 0x108be778
108be7e4  0920a0e1  cpy r2, r9
108be7e8  0a00a0e1  cpy r0, r10
108be7ec  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108be7f0  0210a0e3  mov r1, #0x2
108be7f4  5338feea  b 0x1084c948
108be7f8  0b20a0e1  cpy r2, r11
108be7fc  0210a0e3  mov r1, #0x2
108be800  040086e2  add r0, r6, #0x4
108be804  b23afeeb  bl 0x1084d2d4
108be808  0910a0e1  cpy r1, r9
108be80c  040086e2  add r0, r6, #0x4
108be810  a13afeeb  bl 0x1084d29c
108be814  070084e0  add r0, r4, r7
108be818  0a10d0e5  ldrb r1, [r0,#0xa]
108be81c  040086e2  add r0, r6, #0x4
108be820  9d3afeeb  bl 0x1084d29c
108be824  870084e0  add r0, r4, r7, lsl #0x1
108be828  b010d0e1  ldrh r1, [r0,#0x0]
108be82c  040086e2  add r0, r6, #0x4
108be830  993afeeb  bl 0x1084d29c
108be834  00f020e3  nop
108be838  00f020e3  nop
108be83c  e2ffffea  b 0x108be7cc
108be840  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
