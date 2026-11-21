108ae854  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ae858  0160a0e1  cpy r6, r1
108ae85c  0250a0e1  cpy r5, r2
108ae860  0300a0e3  mov r0, #0x3
108ae864  2cf4feeb  bl 0x1086b91c
108ae868  050050e1  cmp r0, r5
108ae86c  1500001a  bne 0x108ae8c8
108ae870  0520a0e1  cpy r2, r5
108ae874  3410a0e3  mov r1, #0x34
108ae878  0600a0e1  cpy r0, r6
108ae87c  beedfeeb  bl 0x10869f7c
108ae880  0040a0e1  cpy r4, r0
108ae884  0600a0e3  mov r0, #0x6
108ae888  23f4feeb  bl 0x1086b91c
108ae88c  0400c4e5  strb r0, [r4,#0x4]
108ae890  a12fa0e3  mov r2, #0x284
108ae894  0210a0e3  mov r1, #0x2
108ae898  080084e2  add r0, r4, #0x8
108ae89c  8c7afeeb  bl 0x1084d2d4
108ae8a0  0510a0e1  cpy r1, r5
108ae8a4  080084e2  add r0, r4, #0x8
108ae8a8  7b7afeeb  bl 0x1084d29c
108ae8ac  0410d4e5  ldrb r1, [r4,#0x4]
108ae8b0  080084e2  add r0, r4, #0x8
108ae8b4  787afeeb  bl 0x1084d29c
108ae8b8  0410a0e1  cpy r1, r4
108ae8bc  0600a0e1  cpy r0, r6
108ae8c0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ae8c4  9fe4feea  b 0x10867b48
108ae8c8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
