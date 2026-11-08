108bfbe8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bfbec  0260a0e1  cpy r6, r2
108bfbf0  0180a0e1  cpy r8, r1
108bfbf4  0210a0e1  cpy r1, r2
108bfbf8  0800a0e1  cpy r0, r8
108bfbfc  2752feeb  bl 0x108544a0
108bfc00  0070a0e1  cpy r7, r0
108bfc04  bc00d0e1  ldrh r0, [r0,#0xc]
108bfc08  021c40e2  sub r1, r0, #0x200
108bfc0c  2b1051e2  subs r1, r1, #0x2b
108bfc10  2000001a  bne 0x108bfc98
108bfc14  0d10a0e3  mov r1, #0xd
108bfc18  0700a0e1  cpy r0, r7
108bfc1c  ffd1feeb  bl 0x10874420
108bfc20  0040a0e1  cpy r4, r0
108bfc24  0210a0e3  mov r1, #0x2
108bfc28  0700a0e1  cpy r0, r7
108bfc2c  3c31feeb  bl 0x1084c124
108bfc30  000054e1  cmp r4, r0
108bfc34  5301d7e5  ldrb r0, [r7,#0x153]
108bfc38  0150a093  movls r5, #0x1
108bfc3c  0050a083  movhi r5, #0x0
108bfc40  050050e1  cmp r0, r5
108bfc44  1300000a  beq 0x108bfc98
108bfc48  0620a0e1  cpy r2, r6
108bfc4c  3a10a0e3  mov r1, #0x3a
108bfc50  0800a0e1  cpy r0, r8
108bfc54  c8a8feeb  bl 0x10869f7c
108bfc58  001090e5  ldr r1, [r0,#0x0]
108bfc5c  0460c0e5  strb r6, [r0,#0x4]
108bfc60  0550c0e5  strb r5, [r0,#0x5]
108bfc64  021581e3  orr r1, r1, #0x800000
108bfc68  010055e3  cmp r5, #0x1
108bfc6c  001080e5  str r1, [r0,#0x0]
108bfc70  0040a0e1  cpy r4, r0
108bfc74  bf20a003  moveq r2, #0xbf
108bfc78  c020a013  movne r2, #0xc0
108bfc7c  0110a0e3  mov r1, #0x1
108bfc80  080084e2  add r0, r4, #0x8
108bfc84  9235feeb  bl 0x1084d2d4
108bfc88  0410a0e1  cpy r1, r4
108bfc8c  0800a0e1  cpy r0, r8
108bfc90  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bfc94  ab9ffeea  b 0x10867b48
108bfc98  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
