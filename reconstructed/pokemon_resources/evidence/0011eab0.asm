0011eab0  70402de9  stmdb sp!,{r4,r5,r6,lr}
0011eab4  0050a0e1  cpy r5, r0
0011eab8  44609fe5  ldr r6, [0x11eb04]
0011eabc  0140a0e1  cpy r4, r1
0011eac0  000096e5  ldr r0, [r6,#0x0]
0011eac4  000050e3  cmp r0, #0x0
0011eac8  0c00001a  bne 0x0011eb00
0011eacc  0510a0e1  cpy r1, r5
0011ead0  7800a0e3  mov r0, #0x78
0011ead4  910f00eb  bl 0x00122920
0011ead8  000050e3  cmp r0, #0x0
0011eadc  0300000a  beq 0x0011eaf0
0011eae0  0130a0e3  mov r3, #0x1
0011eae4  0420a0e1  cpy r2, r4
0011eae8  0510a0e1  cpy r1, r5
0011eaec  e52500eb  bl 0x00128288
0011eaf0  0510a0e1  cpy r1, r5
0011eaf4  000086e5  str r0, [r6,#0x0]
0011eaf8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
0011eafc  fe2400ea  b 0x00127efc
0011eb00  7080bde8  ldmia sp!,{r4,r5,r6,pc}
