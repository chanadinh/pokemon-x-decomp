108baf70  70402de9  stmdb sp!,{r4,r5,r6,lr}
108baf74  0160a0e1  cpy r6, r1
108baf78  0240a0e1  cpy r4, r2
108baf7c  0300a0e3  mov r0, #0x3
108baf80  65c2feeb  bl 0x1086b91c
108baf84  040050e1  cmp r0, r4
108baf88  1900001a  bne 0x108baff4
108baf8c  0420a0e1  cpy r2, r4
108baf90  1710a0e3  mov r1, #0x17
108baf94  0600a0e1  cpy r0, r6
108baf98  f7bbfeeb  bl 0x10869f7c
108baf9c  0010a0e1  cpy r1, r0
108bafa0  0300a0e3  mov r0, #0x3
108bafa4  0540c1e5  strb r4, [r1,#0x5]
108bafa8  0400c1e5  strb r0, [r1,#0x4]
108bafac  0600a0e1  cpy r0, r6
108bafb0  e4b2feeb  bl 0x10867b48
108bafb4  0420a0e1  cpy r2, r4
108bafb8  0310a0e3  mov r1, #0x3
108bafbc  0600a0e1  cpy r0, r6
108bafc0  edbbfeeb  bl 0x10869f7c
108bafc4  0050a0e1  cpy r5, r0
108bafc8  28209fe5  ldr r2, [0x108baff8]
108bafcc  0210a0e3  mov r1, #0x2
108bafd0  040080e2  add r0, r0, #0x4
108bafd4  be48feeb  bl 0x1084d2d4
108bafd8  0410a0e1  cpy r1, r4
108bafdc  040085e2  add r0, r5, #0x4
108bafe0  ad48feeb  bl 0x1084d29c
108bafe4  0510a0e1  cpy r1, r5
108bafe8  0600a0e1  cpy r0, r6
108bafec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108baff0  d4b2feea  b 0x10867b48
108baff4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
