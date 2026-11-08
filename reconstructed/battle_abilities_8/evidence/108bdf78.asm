108bdf78  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bdf7c  0260a0e1  cpy r6, r2
108bdf80  0170a0e1  cpy r7, r1
108bdf84  0200a0e3  mov r0, #0x2
108bdf88  63b6feeb  bl 0x1086b91c
108bdf8c  ff5000e2  and r5, r0, #0xff
108bdf90  0510a0e1  cpy r1, r5
108bdf94  0600a0e1  cpy r0, r6
108bdf98  8239feeb  bl 0x1084c5a8
108bdf9c  000050e3  cmp r0, #0x0
108bdfa0  4600000a  beq 0x108be0c0
108bdfa4  0610a0e1  cpy r1, r6
108bdfa8  0700a0e1  cpy r0, r7
108bdfac  3b59feeb  bl 0x108544a0
108bdfb0  0090a0e1  cpy r9, r0
108bdfb4  0510a0e1  cpy r1, r5
108bdfb8  0700a0e1  cpy r0, r7
108bdfbc  3759feeb  bl 0x108544a0
108bdfc0  0040a0e1  cpy r4, r0
108bdfc4  0900a0e1  cpy r0, r9
108bdfc8  b201d0e1  ldrh r0, [r0,#0x12]
108bdfcc  000050e3  cmp r0, #0x0
108bdfd0  3a00000a  beq 0x108be0c0
108bdfd4  0400a0e1  cpy r0, r4
108bdfd8  b201d0e1  ldrh r0, [r0,#0x12]
108bdfdc  000050e3  cmp r0, #0x0
108bdfe0  3600001a  bne 0x108be0c0
108bdfe4  0f10a0e3  mov r1, #0xf
108bdfe8  0900a0e1  cpy r0, r9
108bdfec  093afeeb  bl 0x1084c818
108bdff0  000050e3  cmp r0, #0x0
108bdff4  3100001a  bne 0x108be0c0
108bdff8  1110a0e3  mov r1, #0x11
108bdffc  0900a0e1  cpy r0, r9
108be000  043afeeb  bl 0x1084c818
108be004  000050e3  cmp r0, #0x0
108be008  2c00001a  bne 0x108be0c0
108be00c  0900a0e1  cpy r0, r9
108be010  b201d0e1  ldrh r0, [r0,#0x12]
108be014  0080a0e1  cpy r8, r0
108be018  0400a0e1  cpy r0, r4
108be01c  bc00d0e1  ldrh r0, [r0,#0xc]
108be020  0010a0e1  cpy r1, r0
108be024  0820a0e1  cpy r2, r8
108be028  0700a0e1  cpy r0, r7
108be02c  fa58feeb  bl 0x1085441c
108be030  000050e3  cmp r0, #0x0
108be034  2100001a  bne 0x108be0c0
108be038  0900a0e1  cpy r0, r9
108be03c  bc00d0e1  ldrh r0, [r0,#0xc]
108be040  0010a0e1  cpy r1, r0
108be044  0820a0e1  cpy r2, r8
108be048  0700a0e1  cpy r0, r7
108be04c  f258feeb  bl 0x1085441c
108be050  000050e3  cmp r0, #0x0
108be054  1900001a  bne 0x108be0c0
108be058  0620a0e1  cpy r2, r6
108be05c  2510a0e3  mov r1, #0x25
108be060  0700a0e1  cpy r0, r7
108be064  c4affeeb  bl 0x10869f7c
108be068  0040a0e1  cpy r4, r0
108be06c  000090e5  ldr r0, [r0,#0x0]
108be070  0450c4e5  strb r5, [r4,#0x4]
108be074  48209fe5  ldr r2, [0x108be0c4]
108be078  020580e3  orr r0, r0, #0x800000
108be07c  000084e5  str r0, [r4,#0x0]
108be080  0210a0e3  mov r1, #0x2
108be084  080084e2  add r0, r4, #0x8
108be088  913cfeeb  bl 0x1084d2d4
108be08c  0610a0e1  cpy r1, r6
108be090  080084e2  add r0, r4, #0x8
108be094  803cfeeb  bl 0x1084d29c
108be098  0810a0e1  cpy r1, r8
108be09c  080084e2  add r0, r4, #0x8
108be0a0  7d3cfeeb  bl 0x1084d29c
108be0a4  0510a0e1  cpy r1, r5
108be0a8  080084e2  add r0, r4, #0x8
108be0ac  7a3cfeeb  bl 0x1084d29c
108be0b0  0410a0e1  cpy r1, r4
108be0b4  0700a0e1  cpy r0, r7
108be0b8  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108be0bc  a1a6feea  b 0x10867b48
108be0c0  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
