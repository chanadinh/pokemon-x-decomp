108bd460  ff4f2de9  stmdb sp!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,lr}
108bd464  0cd04de2  sub sp, sp, #0xc
108bd468  0200a0e3  mov r0, #0x2
108bd46c  2ab9feeb  bl 0x1086b91c
108bd470  14109de5  ldr r1, [sp,#0x14]
108bd474  010050e1  cmp r0, r1
108bd478  6300001a  bne 0x108bd60c
108bd47c  10009de5  ldr r0, [sp,#0x10]
108bd480  04208de2  add r2, sp, #0x4
108bd484  e435feeb  bl 0x1084ac1c
108bd488  00b0a0e1  cpy r11, r0
108bd48c  0060a0e3  mov r6, #0x0
108bd490  10009de5  ldr r0, [sp,#0x10]
108bd494  0650a0e1  cpy r5, r6
108bd498  1410a0e3  mov r1, #0x14
108bd49c  cb3cfeeb  bl 0x1084c7d0
108bd4a0  00005be3  cmp r11, #0x0
108bd4a4  0080a0e1  cpy r8, r0
108bd4a8  00a0a0e3  mov r10, #0x0
108bd4ac  5600009a  bls 0x108bd60c
108bd4b0  04008de2  add r0, sp, #0x4
108bd4b4  0a10d0e7  ldrb r1, [r0,r10]
108bd4b8  10009de5  ldr r0, [sp,#0x10]
108bd4bc  f75bfeeb  bl 0x108544a0
108bd4c0  00008de5  str r0, [sp,#0x0]
108bd4c4  00f020e3  nop
108bd4c8  5201d0e5  ldrb r0, [r0,#0x152]
108bd4cc  007050e2  subs r7, r0, #0x0
108bd4d0  0040a0e3  mov r4, #0x0
108bd4d4  2800009a  bls 0x108bd57c
108bd4d8  00009de5  ldr r0, [sp,#0x0]
108bd4dc  0410a0e1  cpy r1, r4
108bd4e0  e061feeb  bl 0x10855c68
108bd4e4  0090a0e1  cpy r9, r0
108bd4e8  00f020e3  nop
108bd4ec  ba0cfdeb  bl 0x108007dc
108bd4f0  000050e3  cmp r0, #0x0
108bd4f4  0100a003  moveq r0, #0x1
108bd4f8  1000000a  beq 0x108bd540
108bd4fc  0900a0e1  cpy r0, r9
108bd500  4b10fdeb  bl 0x10801634
108bd504  ff0000e2  and r0, r0, #0xff
108bd508  010050e3  cmp r0, #0x1
108bd50c  0b00001a  bne 0x108bd540
108bd510  0900a0e1  cpy r0, r9
108bd514  ae0cfdeb  bl 0x108007d4
108bd518  090050e3  cmp r0, #0x9
108bd51c  9600a003  moveq r0, #0x96
108bd520  0600000a  beq 0x108bd540
108bd524  440059e3  cmp r9, #0x44
108bd528  f3005913  cmpne r9, #0xf3
108bd52c  0200000a  beq 0x108bd53c
108bd530  170e59e3  cmp r9, #0x170
108bd534  5000a013  movne r0, #0x50
108bd538  0000001a  bne 0x108bd540
108bd53c  7800a0e3  mov r0, #0x78
108bd540  060050e1  cmp r0, r6
108bd544  0060a081  cpyhi r6, r0
108bd548  0050a083  movhi r5, #0x0
108bd54c  0600003a  bcc 0x108bd56c
108bd550  00009de5  ldr r0, [sp,#0x0]
108bd554  1900d0e5  ldrb r0, [r0,#0x19]
108bd558  051188e0  add r1, r8, r5, lsl #0x2
108bd55c  0501c8e7  strb r0, [r8,r5,lsl #0x2]
108bd560  012085e2  add r2, r5, #0x1
108bd564  ff5002e2  and r5, r2, #0xff
108bd568  b290c1e1  strh r9, [r1,#0x2]
108bd56c  010084e2  add r0, r4, #0x1
108bd570  ff4000e2  and r4, r0, #0xff
108bd574  070054e1  cmp r4, r7
108bd578  d6ffff3a  bcc 0x108bd4d8
108bd57c  01008ae2  add r0, r10, #0x1
108bd580  ffa000e2  and r10, r0, #0xff
108bd584  0a005be1  cmp r11, r10
108bd588  c8ffff8a  bhi 0x108bd4b0
108bd58c  000055e3  cmp r5, #0x0
108bd590  1d00000a  beq 0x108bd60c
108bd594  0500a0e1  cpy r0, r5
108bd598  873cfeeb  bl 0x1084c7bc
108bd59c  10308de2  add r3, sp, #0x10
108bd5a0  ff5000e2  and r5, r0, #0xff
108bd5a4  050093e8  ldmia r3,{r0,r2}
108bd5a8  0110a0e3  mov r1, #0x1
108bd5ac  e53cfeeb  bl 0x1084c948
108bd5b0  10308de2  add r3, sp, #0x10
108bd5b4  0310a0e3  mov r1, #0x3
108bd5b8  050093e8  ldmia r3,{r0,r2}
108bd5bc  6eb2feeb  bl 0x10869f7c
108bd5c0  0040a0e1  cpy r4, r0
108bd5c4  48209fe5  ldr r2, [0x108bd614]
108bd5c8  0210a0e3  mov r1, #0x2
108bd5cc  040080e2  add r0, r0, #0x4
108bd5d0  3f3ffeeb  bl 0x1084d2d4
108bd5d4  0511d8e7  ldrb r1, [r8,r5,lsl #0x2]
108bd5d8  040084e2  add r0, r4, #0x4
108bd5dc  2e3ffeeb  bl 0x1084d29c
108bd5e0  050188e0  add r0, r8, r5, lsl #0x2
108bd5e4  b210d0e1  ldrh r1, [r0,#0x2]
108bd5e8  040084e2  add r0, r4, #0x4
108bd5ec  2a3ffeeb  bl 0x1084d29c
108bd5f0  10009de5  ldr r0, [sp,#0x10]
108bd5f4  0410a0e1  cpy r1, r4
108bd5f8  52a9feeb  bl 0x10867b48
108bd5fc  10308de2  add r3, sp, #0x10
108bd600  0210a0e3  mov r1, #0x2
108bd604  050093e8  ldmia r3,{r0,r2}
108bd608  ce3cfeeb  bl 0x1084c948
108bd60c  1cd08de2  add sp, sp, #0x1c
108bd610  f08fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,pc}
