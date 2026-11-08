108ba62c  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108ba630  0340a0e1  cpy r4, r3
108ba634  0170a0e1  cpy r7, r1
108ba638  0260a0e1  cpy r6, r2
108ba63c  0200a0e3  mov r0, #0x2
108ba640  b5c4feeb  bl 0x1086b91c
108ba644  060050e1  cmp r0, r6
108ba648  4900001a  bne 0x108ba774
108ba64c  0e00a0e3  mov r0, #0xe
108ba650  b1c4feeb  bl 0x1086b91c
108ba654  ff5000e2  and r5, r0, #0xff
108ba658  0a0055e3  cmp r5, #0xa
108ba65c  0900001a  bne 0x108ba688
108ba660  0610a0e1  cpy r1, r6
108ba664  0700a0e1  cpy r0, r7
108ba668  4249feeb  bl 0x1084cb78
108ba66c  0050a0e1  cpy r5, r0
108ba670  0700a0e1  cpy r0, r7
108ba674  2646feeb  bl 0x1084bf14
108ba678  0020a0e3  mov r2, #0x0
108ba67c  0510a0e1  cpy r1, r5
108ba680  e5cdfeeb  bl 0x1086de1c
108ba684  0050a0e1  cpy r5, r0
108ba688  0420a0e1  cpy r2, r4
108ba68c  0510a0e1  cpy r1, r5
108ba690  0700a0e1  cpy r0, r7
108ba694  6145feeb  bl 0x1084bc20
108ba698  000050e3  cmp r0, #0x0
108ba69c  00f020e3  nop
108ba6a0  2900000a  beq 0x108ba74c
108ba6a4  0010d4e5  ldrb r1, [r4,#0x0]
108ba6a8  0700a0e1  cpy r0, r7
108ba6ac  7b67feeb  bl 0x108544a0
108ba6b0  0040d4e5  ldrb r4, [r4,#0x0]
108ba6b4  0310a0e3  mov r1, #0x3
108ba6b8  5648feeb  bl 0x1084c818
108ba6bc  000050e3  cmp r0, #0x0
108ba6c0  00f020e3  nop
108ba6c4  2000001a  bne 0x108ba74c
108ba6c8  0d20a0e1  cpy r2, sp
108ba6cc  0410a0e1  cpy r1, r4
108ba6d0  0700a0e1  cpy r0, r7
108ba6d4  2a44feeb  bl 0x1084b784
108ba6d8  000050e3  cmp r0, #0x0
108ba6dc  00f020e3  nop
108ba6e0  1900000a  beq 0x108ba74c
108ba6e4  0410a0e1  cpy r1, r4
108ba6e8  0700a0e1  cpy r0, r7
108ba6ec  2b46feeb  bl 0x1084bfa0
108ba6f0  000050e3  cmp r0, #0x0
108ba6f4  00f020e3  nop
108ba6f8  1300000a  beq 0x108ba74c
108ba6fc  0410a0e1  cpy r1, r4
108ba700  0700a0e1  cpy r0, r7
108ba704  4946feeb  bl 0x1084c030
108ba708  000050e3  cmp r0, #0x0
108ba70c  00f020e3  nop
108ba710  0d00001a  bne 0x108ba74c
108ba714  0d00a0e1  cpy r0, sp
108ba718  ca43feeb  bl 0x1084b648
108ba71c  0040b0e1  movs r4, r0
108ba720  00f020e3  nop
108ba724  0800000a  beq 0x108ba74c
108ba728  4117fdeb  bl 0x10800434
108ba72c  000050e3  cmp r0, #0x0
108ba730  00f020e3  nop
108ba734  0400000a  beq 0x108ba74c
108ba738  0400a0e1  cpy r0, r4
108ba73c  38a8ffeb  bl 0x108a4824
108ba740  000050e3  cmp r0, #0x0
108ba744  00f020e3  nop
108ba748  0300000a  beq 0x108ba75c
108ba74c  f840bde8  ldmia sp!,{r3,r4,r5,r6,r7,lr}
108ba750  0110a0e3  mov r1, #0x1
108ba754  4200a0e3  mov r0, #0x42
108ba758  5dacfeea  b 0x108658d4
108ba75c  0410a0e1  cpy r1, r4
108ba760  1200a0e3  mov r0, #0x12
108ba764  5aacfeeb  bl 0x108658d4
108ba768  0510a0e1  cpy r1, r5
108ba76c  0d00a0e3  mov r0, #0xd
108ba770  57acfeeb  bl 0x108658d4
108ba774  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
