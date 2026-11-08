1084aba4  70402de9  stmdb sp!,{r4,r5,r6,lr}
1084aba8  0240a0e1  cpy r4, r2
1084abac  0060a0e1  cpy r6, r0
1084abb0  0150a0e1  cpy r5, r1
1084abb4  392600eb  bl 0x108544a0
1084abb8  0410a0e1  cpy r1, r4
1084abbc  b12f00eb  bl 0x10856a88
1084abc0  000050e3  cmp r0, #0x0
1084abc4  1300000a  beq 0x1084ac18
1084abc8  0520a0e1  cpy r2, r5
1084abcc  0110a0e3  mov r1, #0x1
1084abd0  0600a0e1  cpy r0, r6
1084abd4  5b0700eb  bl 0x1084c948
1084abd8  0520a0e1  cpy r2, r5
1084abdc  0a10a0e3  mov r1, #0xa
1084abe0  0600a0e1  cpy r0, r6
1084abe4  e47c00eb  bl 0x10869f7c
1084abe8  0010a0e1  cpy r1, r0
1084abec  0100a0e3  mov r0, #0x1
1084abf0  0550c1e5  strb r5, [r1,#0x5]
1084abf4  0440c1e5  strb r4, [r1,#0x4]
1084abf8  1100c1e5  strb r0, [r1,#0x11]
1084abfc  0600a0e1  cpy r0, r6
1084ac00  d07300eb  bl 0x10867b48
1084ac04  0520a0e1  cpy r2, r5
1084ac08  0600a0e1  cpy r0, r6
1084ac0c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
1084ac10  0210a0e3  mov r1, #0x2
1084ac14  4b0700ea  b 0x1084c948
1084ac18  7080bde8  ldmia sp!,{r4,r5,r6,pc}
