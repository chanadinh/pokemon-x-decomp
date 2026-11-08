108aaf64  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aaf68  0060a0e1  cpy r6, r0
108aaf6c  0150a0e1  cpy r5, r1
108aaf70  0240a0e1  cpy r4, r2
108aaf74  0170a0e3  mov r7, #0x1
108aaf78  0400a0e3  mov r0, #0x4
108aaf7c  6602ffeb  bl 0x1086b91c
108aaf80  040050e1  cmp r0, r4
108aaf84  1600001a  bne 0x108aafe4
108aaf88  4700a0e3  mov r0, #0x47
108aaf8c  6202ffeb  bl 0x1086b91c
108aaf90  000050e3  cmp r0, #0x0
108aaf94  1200001a  bne 0x108aafe4
108aaf98  1600a0e3  mov r0, #0x16
108aaf9c  5e02ffeb  bl 0x1086b91c
108aafa0  0c0050e3  cmp r0, #0xc
108aafa4  0e00001a  bne 0x108aafe4
108aafa8  0410a0e1  cpy r1, r4
108aafac  0500a0e1  cpy r0, r5
108aafb0  3aa5feeb  bl 0x108544a0
108aafb4  0120a0e3  mov r2, #0x1
108aafb8  0710a0e1  cpy r1, r7
108aafbc  c91cffeb  bl 0x108722e8
108aafc0  000050e3  cmp r0, #0x0
108aafc4  0600000a  beq 0x108aafe4
108aafc8  0600a0e1  cpy r0, r6
108aafcc  6686feeb  bl 0x1084c96c
108aafd0  0420a0e1  cpy r2, r4
108aafd4  0500a0e1  cpy r0, r5
108aafd8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aafdc  0010a0e3  mov r1, #0x0
108aafe0  5886feea  b 0x1084c948
108aafe4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
