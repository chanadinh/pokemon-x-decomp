108baabc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108baac0  0160a0e1  cpy r6, r1
108baac4  0250a0e1  cpy r5, r2
108baac8  0300a0e3  mov r0, #0x3
108baacc  92c3feeb  bl 0x1086b91c
108baad0  050050e1  cmp r0, r5
108baad4  0f00001a  bne 0x108bab18
108baad8  0520a0e1  cpy r2, r5
108baadc  0310a0e3  mov r1, #0x3
108baae0  0600a0e1  cpy r0, r6
108baae4  24bdfeeb  bl 0x10869f7c
108baae8  0040a0e1  cpy r4, r0
108baaec  28209fe5  ldr r2, [0x108bab1c]
108baaf0  0210a0e3  mov r1, #0x2
108baaf4  040080e2  add r0, r0, #0x4
108baaf8  f549feeb  bl 0x1084d2d4
108baafc  0510a0e1  cpy r1, r5
108bab00  040084e2  add r0, r4, #0x4
108bab04  e449feeb  bl 0x1084d29c
108bab08  0410a0e1  cpy r1, r4
108bab0c  0600a0e1  cpy r0, r6
108bab10  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bab14  0bb4feea  b 0x10867b48
108bab18  7080bde8  ldmia sp!,{r4,r5,r6,pc}
