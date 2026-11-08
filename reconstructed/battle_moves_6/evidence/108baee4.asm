108baee4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108baee8  0160a0e1  cpy r6, r1
108baeec  0240a0e1  cpy r4, r2
108baef0  0300a0e3  mov r0, #0x3
108baef4  88c2feeb  bl 0x1086b91c
108baef8  040050e1  cmp r0, r4
108baefc  1900001a  bne 0x108baf68
108baf00  0420a0e1  cpy r2, r4
108baf04  1710a0e3  mov r1, #0x17
108baf08  0600a0e1  cpy r0, r6
108baf0c  1abcfeeb  bl 0x10869f7c
108baf10  0010a0e1  cpy r1, r0
108baf14  0600a0e3  mov r0, #0x6
108baf18  0540c1e5  strb r4, [r1,#0x5]
108baf1c  0400c1e5  strb r0, [r1,#0x4]
108baf20  0600a0e1  cpy r0, r6
108baf24  07b3feeb  bl 0x10867b48
108baf28  0420a0e1  cpy r2, r4
108baf2c  0310a0e3  mov r1, #0x3
108baf30  0600a0e1  cpy r0, r6
108baf34  10bcfeeb  bl 0x10869f7c
108baf38  0050a0e1  cpy r5, r0
108baf3c  28209fe5  ldr r2, [0x108baf6c]
108baf40  0210a0e3  mov r1, #0x2
108baf44  040080e2  add r0, r0, #0x4
108baf48  e148feeb  bl 0x1084d2d4
108baf4c  0410a0e1  cpy r1, r4
108baf50  040085e2  add r0, r5, #0x4
108baf54  d048feeb  bl 0x1084d29c
108baf58  0510a0e1  cpy r1, r5
108baf5c  0600a0e1  cpy r0, r6
108baf60  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108baf64  f7b2feea  b 0x10867b48
108baf68  7080bde8  ldmia sp!,{r4,r5,r6,pc}
