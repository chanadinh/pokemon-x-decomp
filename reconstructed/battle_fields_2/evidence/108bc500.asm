108bc500  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bc504  0160a0e1  cpy r6, r1
108bc508  0200a0e3  mov r0, #0x2
108bc50c  02bdfeeb  bl 0x1086b91c
108bc510  ff4000e2  and r4, r0, #0xff
108bc514  1f0054e3  cmp r4, #0x1f
108bc518  2200000a  beq 0x108bc5a8
108bc51c  0410a0e1  cpy r1, r4
108bc520  0600a0e1  cpy r0, r6
108bc524  7e3efeeb  bl 0x1084bf24
108bc528  000050e3  cmp r0, #0x0
108bc52c  1d00001a  bne 0x108bc5a8
108bc530  0410a0e1  cpy r1, r4
108bc534  0600a0e1  cpy r0, r6
108bc538  7c3afeeb  bl 0x1084af30
108bc53c  000050e3  cmp r0, #0x0
108bc540  1800001a  bne 0x108bc5a8
108bc544  0410a0e1  cpy r1, r4
108bc548  0600a0e1  cpy r0, r6
108bc54c  d35ffeeb  bl 0x108544a0
108bc550  0070a0e1  cpy r7, r0
108bc554  0420a0e1  cpy r2, r4
108bc558  0410a0e3  mov r1, #0x4
108bc55c  0600a0e1  cpy r0, r6
108bc560  85b6feeb  bl 0x10869f7c
108bc564  0050a0e1  cpy r5, r0
108bc568  0640c0e5  strb r4, [r0,#0x6]
108bc56c  1010a0e3  mov r1, #0x10
108bc570  0700a0e1  cpy r0, r7
108bc574  5e43feeb  bl 0x1084d2f4
108bc578  b400c5e1  strh r0, [r5,#0x4]
108bc57c  28209fe5  ldr r2, [0x108bc5ac]
108bc580  0210a0e3  mov r1, #0x2
108bc584  080085e2  add r0, r5, #0x8
108bc588  5143feeb  bl 0x1084d2d4
108bc58c  0410a0e1  cpy r1, r4
108bc590  080085e2  add r0, r5, #0x8
108bc594  4043feeb  bl 0x1084d29c
108bc598  0510a0e1  cpy r1, r5
108bc59c  0600a0e1  cpy r0, r6
108bc5a0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bc5a4  67adfeea  b 0x10867b48
108bc5a8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
