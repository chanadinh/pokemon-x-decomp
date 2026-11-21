108ae754  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108ae758  0300a0e3  mov r0, #0x3
108ae75c  0280a0e1  cpy r8, r2
108ae760  0190a0e1  cpy r9, r1
108ae764  6cf4feeb  bl 0x1086b91c
108ae768  080050e1  cmp r0, r8
108ae76c  2100001a  bne 0x108ae7f8
108ae770  0500a0e3  mov r0, #0x5
108ae774  68f4feeb  bl 0x1086b91c
108ae778  007050e2  subs r7, r0, #0x0
108ae77c  78b09f85  ldrhi r11, [0x108ae7fc]
108ae780  0050a0e3  mov r5, #0x0
108ae784  25a0a083  movhi r10, #0x25
108ae788  1a00009a  bls 0x108ae7f8
108ae78c  060085e2  add r0, r5, #0x6
108ae790  ff0000e2  and r0, r0, #0xff
108ae794  60f4feeb  bl 0x1086b91c
108ae798  ff6000e2  and r6, r0, #0xff
108ae79c  0820a0e1  cpy r2, r8
108ae7a0  0b10a0e3  mov r1, #0xb
108ae7a4  0900a0e1  cpy r0, r9
108ae7a8  f3edfeeb  bl 0x10869f7c
108ae7ac  0f60c0e5  strb r6, [r0,#0xf]
108ae7b0  0040a0e1  cpy r4, r0
108ae7b4  b4a0c0e1  strh r10, [r0,#0x4]
108ae7b8  0100a0e3  mov r0, #0x1
108ae7bc  9e75feeb  bl 0x1084be3c
108ae7c0  080084e5  str r0, [r4,#0x8]
108ae7c4  0b20a0e1  cpy r2, r11
108ae7c8  0210a0e3  mov r1, #0x2
108ae7cc  140084e2  add r0, r4, #0x14
108ae7d0  bf7afeeb  bl 0x1084d2d4
108ae7d4  0610a0e1  cpy r1, r6
108ae7d8  140084e2  add r0, r4, #0x14
108ae7dc  ae7afeeb  bl 0x1084d29c
108ae7e0  0410a0e1  cpy r1, r4
108ae7e4  0900a0e1  cpy r0, r9
108ae7e8  d6e4feeb  bl 0x10867b48
108ae7ec  015085e2  add r5, r5, #0x1
108ae7f0  050057e1  cmp r7, r5
108ae7f4  e4ffff8a  bhi 0x108ae78c
108ae7f8  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
