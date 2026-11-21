108ba3cc  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108ba3d0  0240a0e1  cpy r4, r2
108ba3d4  0170a0e1  cpy r7, r1
108ba3d8  0360a0e1  cpy r6, r3
108ba3dc  0300a0e3  mov r0, #0x3
108ba3e0  4dc5feeb  bl 0x1086b91c
108ba3e4  040050e1  cmp r0, r4
108ba3e8  2b00001a  bne 0x108ba49c
108ba3ec  0410a0e1  cpy r1, r4
108ba3f0  0700a0e1  cpy r0, r7
108ba3f4  2968feeb  bl 0x108544a0
108ba3f8  b201d0e1  ldrh r0, [r0,#0x12]
108ba3fc  0050b0e1  movs r5, r0
108ba400  2500000a  beq 0x108ba49c
108ba404  0420a0e1  cpy r2, r4
108ba408  2410a0e3  mov r1, #0x24
108ba40c  0700a0e1  cpy r0, r7
108ba410  d9befeeb  bl 0x10869f7c
108ba414  0180a0e3  mov r8, #0x1
108ba418  0480c0e5  strb r8, [r0,#0x4]
108ba41c  0010a0e1  cpy r1, r0
108ba420  0580c0e5  strb r8, [r0,#0x5]
108ba424  0700a0e1  cpy r0, r7
108ba428  c6b5feeb  bl 0x10867b48
108ba42c  4700a0e3  mov r0, #0x47
108ba430  008086e5  str r8, [r6,#0x0]
108ba434  38c5feeb  bl 0x1086b91c
108ba438  000050e3  cmp r0, #0x0
108ba43c  1600001a  bne 0x108ba49c
108ba440  4800a0e3  mov r0, #0x48
108ba444  34c5feeb  bl 0x1086b91c
108ba448  000050e3  cmp r0, #0x0
108ba44c  1200001a  bne 0x108ba49c
108ba450  0510a0e3  mov r1, #0x5
108ba454  0500a0e1  cpy r0, r5
108ba458  c54afeeb  bl 0x1084cf74
108ba45c  000050e3  cmp r0, #0x0
108ba460  0d00000a  beq 0x108ba49c
108ba464  0420a0e1  cpy r2, r4
108ba468  2310a0e3  mov r1, #0x23
108ba46c  0700a0e1  cpy r0, r7
108ba470  c1befeeb  bl 0x10869f7c
108ba474  0040a0e1  cpy r4, r0
108ba478  0400a0e3  mov r0, #0x4
108ba47c  26c5feeb  bl 0x1086b91c
108ba480  0400c4e5  strb r0, [r4,#0x4]
108ba484  b650c4e1  strh r5, [r4,#0x6]
108ba488  0410a0e1  cpy r1, r4
108ba48c  0700a0e1  cpy r0, r7
108ba490  0580c4e5  strb r8, [r4,#0x5]
108ba494  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108ba498  aab5feea  b 0x10867b48
108ba49c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
