108baffc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb000  0160a0e1  cpy r6, r1
108bb004  0240a0e1  cpy r4, r2
108bb008  0300a0e3  mov r0, #0x3
108bb00c  42c2feeb  bl 0x1086b91c
108bb010  040050e1  cmp r0, r4
108bb014  1900001a  bne 0x108bb080
108bb018  0420a0e1  cpy r2, r4
108bb01c  1710a0e3  mov r1, #0x17
108bb020  0600a0e1  cpy r0, r6
108bb024  d4bbfeeb  bl 0x10869f7c
108bb028  0010a0e1  cpy r1, r0
108bb02c  0300a0e3  mov r0, #0x3
108bb030  0540c1e5  strb r4, [r1,#0x5]
108bb034  0400c1e5  strb r0, [r1,#0x4]
108bb038  0600a0e1  cpy r0, r6
108bb03c  c1b2feeb  bl 0x10867b48
108bb040  0420a0e1  cpy r2, r4
108bb044  0310a0e3  mov r1, #0x3
108bb048  0600a0e1  cpy r0, r6
108bb04c  cabbfeeb  bl 0x10869f7c
108bb050  0050a0e1  cpy r5, r0
108bb054  222ea0e3  mov r2, #0x220
108bb058  0210a0e3  mov r1, #0x2
108bb05c  040080e2  add r0, r0, #0x4
108bb060  9b48feeb  bl 0x1084d2d4
108bb064  0410a0e1  cpy r1, r4
108bb068  040085e2  add r0, r5, #0x4
108bb06c  8a48feeb  bl 0x1084d29c
108bb070  0510a0e1  cpy r1, r5
108bb074  0600a0e1  cpy r0, r6
108bb078  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb07c  b1b2feea  b 0x10867b48
108bb080  7080bde8  ldmia sp!,{r4,r5,r6,pc}
