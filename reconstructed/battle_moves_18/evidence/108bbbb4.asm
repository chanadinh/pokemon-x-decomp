108bbbb4  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bbbb8  0250a0e1  cpy r5, r2
108bbbbc  0170a0e1  cpy r7, r1
108bbbc0  0300a0e3  mov r0, #0x3
108bbbc4  54bffeeb  bl 0x1086b91c
108bbbc8  050050e1  cmp r0, r5
108bbbcc  2200001a  bne 0x108bbc5c
108bbbd0  0510a0e1  cpy r1, r5
108bbbd4  0700a0e1  cpy r0, r7
108bbbd8  3062feeb  bl 0x108544a0
108bbbdc  b201d0e1  ldrh r0, [r0,#0x12]
108bbbe0  0060b0e1  movs r6, r0
108bbbe4  1c00000a  beq 0x108bbc5c
108bbbe8  0520a0e1  cpy r2, r5
108bbbec  0310a0e3  mov r1, #0x3
108bbbf0  0700a0e1  cpy r0, r7
108bbbf4  e0b8feeb  bl 0x10869f7c
108bbbf8  0040a0e1  cpy r4, r0
108bbbfc  5c209fe5  ldr r2, [0x108bbc60]
108bbc00  0210a0e3  mov r1, #0x2
108bbc04  040080e2  add r0, r0, #0x4
108bbc08  b145feeb  bl 0x1084d2d4
108bbc0c  0510a0e1  cpy r1, r5
108bbc10  040084e2  add r0, r4, #0x4
108bbc14  a045feeb  bl 0x1084d29c
108bbc18  0610a0e1  cpy r1, r6
108bbc1c  040084e2  add r0, r4, #0x4
108bbc20  9d45feeb  bl 0x1084d29c
108bbc24  0410a0e1  cpy r1, r4
108bbc28  0700a0e1  cpy r0, r7
108bbc2c  c5affeeb  bl 0x10867b48
108bbc30  0520a0e1  cpy r2, r5
108bbc34  1510a0e3  mov r1, #0x15
108bbc38  0700a0e1  cpy r0, r7
108bbc3c  ceb8feeb  bl 0x10869f7c
108bbc40  0010a0e1  cpy r1, r0
108bbc44  0f00a0e3  mov r0, #0xf
108bbc48  0550c1e5  strb r5, [r1,#0x5]
108bbc4c  0400c1e5  strb r0, [r1,#0x4]
108bbc50  0700a0e1  cpy r0, r7
108bbc54  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bbc58  baaffeea  b 0x10867b48
108bbc5c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
