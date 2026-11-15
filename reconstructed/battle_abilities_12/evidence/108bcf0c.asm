108bcf0c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bcf10  0250a0e1  cpy r5, r2
108bcf14  0170a0e1  cpy r7, r1
108bcf18  0200a0e3  mov r0, #0x2
108bcf1c  7ebafeeb  bl 0x1086b91c
108bcf20  050050e1  cmp r0, r5
108bcf24  3000001a  bne 0x108bcfec
108bcf28  0510a0e1  cpy r1, r5
108bcf2c  0700a0e1  cpy r0, r7
108bcf30  88d8feeb  bl 0x10873158
108bcf34  0040a0e1  cpy r4, r0
108bcf38  0700a0e1  cpy r0, r7
108bcf3c  f43bfeeb  bl 0x1084bf14
108bcf40  0410a0e1  cpy r1, r4
108bcf44  a4c3feeb  bl 0x1086dddc
108bcf48  0010a0e1  cpy r1, r0
108bcf4c  0700a0e1  cpy r0, r7
108bcf50  fc40feeb  bl 0x1084d348
108bcf54  0060a0e1  cpy r6, r0
108bcf58  0010a0e1  cpy r1, r0
108bcf5c  0700a0e1  cpy r0, r7
108bcf60  4e5dfeeb  bl 0x108544a0
108bcf64  0040a0e1  cpy r4, r0
108bcf68  0510a0e1  cpy r1, r5
108bcf6c  0700a0e1  cpy r0, r7
108bcf70  4a5dfeeb  bl 0x108544a0
108bcf74  328afeeb  bl 0x1085f844
108bcf78  000050e3  cmp r0, #0x0
108bcf7c  1a00001a  bne 0x108bcfec
108bcf80  0400a0e1  cpy r0, r4
108bcf84  2e8afeeb  bl 0x1085f844
108bcf88  000050e3  cmp r0, #0x0
108bcf8c  1600001a  bne 0x108bcfec
108bcf90  0520a0e1  cpy r2, r5
108bcf94  3410a0e3  mov r1, #0x34
108bcf98  0700a0e1  cpy r0, r7
108bcf9c  f6b3feeb  bl 0x10869f7c
108bcfa0  0040a0e1  cpy r4, r0
108bcfa4  000090e5  ldr r0, [r0,#0x0]
108bcfa8  0460c4e5  strb r6, [r4,#0x4]
108bcfac  a12fa0e3  mov r2, #0x284
108bcfb0  020580e3  orr r0, r0, #0x800000
108bcfb4  000084e5  str r0, [r4,#0x0]
108bcfb8  0210a0e3  mov r1, #0x2
108bcfbc  080084e2  add r0, r4, #0x8
108bcfc0  c340feeb  bl 0x1084d2d4
108bcfc4  0510a0e1  cpy r1, r5
108bcfc8  080084e2  add r0, r4, #0x8
108bcfcc  b240feeb  bl 0x1084d29c
108bcfd0  0410d4e5  ldrb r1, [r4,#0x4]
108bcfd4  080084e2  add r0, r4, #0x8
108bcfd8  af40feeb  bl 0x1084d29c
108bcfdc  0410a0e1  cpy r1, r4
108bcfe0  0700a0e1  cpy r0, r7
108bcfe4  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bcfe8  d6aafeea  b 0x10867b48
108bcfec  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
