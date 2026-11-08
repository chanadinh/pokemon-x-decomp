108abbec  70402de9  stmdb sp!,{r4,r5,r6,lr}
108abbf0  0160a0e1  cpy r6, r1
108abbf4  0250a0e1  cpy r5, r2
108abbf8  0400a0e3  mov r0, #0x4
108abbfc  46fffeeb  bl 0x1086b91c
108abc00  050050e1  cmp r0, r5
108abc04  1c00001a  bne 0x108abc7c
108abc08  4700a0e3  mov r0, #0x47
108abc0c  42fffeeb  bl 0x1086b91c
108abc10  000050e3  cmp r0, #0x0
108abc14  1800001a  bne 0x108abc7c
108abc18  1200a0e3  mov r0, #0x12
108abc1c  3efffeeb  bl 0x1086b91c
108abc20  7000ffe6  uxth r0, r0
108abc24  0010a0e3  mov r1, #0x0
108abc28  1b52fdeb  bl 0x1080049c
108abc2c  000050e3  cmp r0, #0x0
108abc30  1100000a  beq 0x108abc7c
108abc34  0300a0e3  mov r0, #0x3
108abc38  37fffeeb  bl 0x1086b91c
108abc3c  ff4000e2  and r4, r0, #0xff
108abc40  0410a0e1  cpy r1, r4
108abc44  0600a0e1  cpy r0, r6
108abc48  14a2feeb  bl 0x108544a0
108abc4c  b201d0e1  ldrh r0, [r0,#0x12]
108abc50  000050e3  cmp r0, #0x0
108abc54  0800001a  bne 0x108abc7c
108abc58  0520a0e1  cpy r2, r5
108abc5c  2510a0e3  mov r1, #0x25
108abc60  0600a0e1  cpy r0, r6
108abc64  c4f8feeb  bl 0x10869f7c
108abc68  0010a0e1  cpy r1, r0
108abc6c  0440c0e5  strb r4, [r0,#0x4]
108abc70  0600a0e1  cpy r0, r6
108abc74  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108abc78  b2effeea  b 0x10867b48
108abc7c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
