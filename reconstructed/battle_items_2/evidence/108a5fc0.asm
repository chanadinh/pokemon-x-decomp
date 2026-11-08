108a5fc0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a5fc4  0160a0e1  cpy r6, r1
108a5fc8  0070a0e1  cpy r7, r0
108a5fcc  0250a0e1  cpy r5, r2
108a5fd0  0200a0e3  mov r0, #0x2
108a5fd4  5016ffeb  bl 0x1086b91c
108a5fd8  050050e1  cmp r0, r5
108a5fdc  2500001a  bne 0x108a6078
108a5fe0  0510a0e1  cpy r1, r5
108a5fe4  0600a0e1  cpy r0, r6
108a5fe8  2cb9feeb  bl 0x108544a0
108a5fec  0910a0e3  mov r1, #0x9
108a5ff0  959afeeb  bl 0x1084ca4c
108a5ff4  000050e3  cmp r0, #0x0
108a5ff8  1e00001a  bne 0x108a6078
108a5ffc  0520a0e1  cpy r2, r5
108a6000  1710a0e3  mov r1, #0x17
108a6004  0600a0e1  cpy r0, r6
108a6008  db0fffeb  bl 0x10869f7c
108a600c  0010a0e1  cpy r1, r0
108a6010  0900a0e3  mov r0, #0x9
108a6014  0550c1e5  strb r5, [r1,#0x5]
108a6018  0400c1e5  strb r0, [r1,#0x4]
108a601c  0600a0e1  cpy r0, r6
108a6020  c806ffeb  bl 0x10867b48
108a6024  0520a0e1  cpy r2, r5
108a6028  0310a0e3  mov r1, #0x3
108a602c  0600a0e1  cpy r0, r6
108a6030  d10fffeb  bl 0x10869f7c
108a6034  0040a0e1  cpy r4, r0
108a6038  3c209fe5  ldr r2, [0x108a607c]
108a603c  0210a0e3  mov r1, #0x2
108a6040  040080e2  add r0, r0, #0x4
108a6044  a29cfeeb  bl 0x1084d2d4
108a6048  0510a0e1  cpy r1, r5
108a604c  040084e2  add r0, r4, #0x4
108a6050  919cfeeb  bl 0x1084d29c
108a6054  0700a0e1  cpy r0, r7
108a6058  b803d0e1  ldrh r0, [r0,#0x38]
108a605c  0010a0e1  cpy r1, r0
108a6060  040084e2  add r0, r4, #0x4
108a6064  8c9cfeeb  bl 0x1084d29c
108a6068  0410a0e1  cpy r1, r4
108a606c  0600a0e1  cpy r0, r6
108a6070  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a6074  b306ffea  b 0x10867b48
108a6078  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
