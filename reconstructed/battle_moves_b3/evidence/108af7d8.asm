108af7d8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108af7dc  0160a0e1  cpy r6, r1
108af7e0  0250a0e1  cpy r5, r2
108af7e4  0370a0e1  cpy r7, r3
108af7e8  0300a0e3  mov r0, #0x3
108af7ec  4af0feeb  bl 0x1086b91c
108af7f0  050050e1  cmp r0, r5
108af7f4  2800001a  bne 0x108af89c
108af7f8  0520a0e1  cpy r2, r5
108af7fc  0310a0e3  mov r1, #0x3
108af800  0600a0e1  cpy r0, r6
108af804  dce9feeb  bl 0x10869f7c
108af808  0040a0e1  cpy r4, r0
108af80c  8c209fe5  ldr r2, [0x108af8a0]
108af810  0210a0e3  mov r1, #0x2
108af814  040080e2  add r0, r0, #0x4
108af818  ad76feeb  bl 0x1084d2d4
108af81c  0510a0e1  cpy r1, r5
108af820  040084e2  add r0, r4, #0x4
108af824  9c76feeb  bl 0x1084d29c
108af828  0410a0e1  cpy r1, r4
108af82c  0600a0e1  cpy r0, r6
108af830  c4e0feeb  bl 0x10867b48
108af834  0100a0e3  mov r0, #0x1
108af838  180087e5  str r0, [r7,#0x18]
108af83c  0510a0e1  cpy r1, r5
108af840  0600a0e1  cpy r0, r6
108af844  1593feeb  bl 0x108544a0
108af848  0070a0e1  cpy r7, r0
108af84c  0520a0e1  cpy r2, r5
108af850  2710a0e3  mov r1, #0x27
108af854  0600a0e1  cpy r0, r6
108af858  c7e9feeb  bl 0x10869f7c
108af85c  0040a0e1  cpy r4, r0
108af860  0310a0e3  mov r1, #0x3
108af864  0700a0e1  cpy r0, r7
108af868  af70feeb  bl 0x1084bb2c
108af86c  010080e2  add r0, r0, #0x1
108af870  0600c4e5  strb r0, [r4,#0x6]
108af874  0310a0e3  mov r1, #0x3
108af878  0450c4e5  strb r5, [r4,#0x4]
108af87c  0510c4e5  strb r1, [r4,#0x5]
108af880  000094e5  ldr r0, [r4,#0x0]
108af884  0410a0e1  cpy r1, r4
108af888  0104c0e3  bic r0, r0, #0x1000000
108af88c  000084e5  str r0, [r4,#0x0]
108af890  0600a0e1  cpy r0, r6
108af894  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108af898  aae0feea  b 0x10867b48
108af89c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
