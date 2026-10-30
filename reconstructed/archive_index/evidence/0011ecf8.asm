0011ecf8  70402de9  stmdb sp!,{r4,r5,r6,lr}
0011ecfc  0050a0e1  cpy r5, r0
0011ed00  44609fe5  ldr r6, [0x11ed4c]
0011ed04  0140a0e1  cpy r4, r1
0011ed08  000096e5  ldr r0, [r6,#0x0]
0011ed0c  000050e3  cmp r0, #0x0
0011ed10  0c00001a  bne 0x0011ed48
0011ed14  0510a0e1  cpy r1, r5
0011ed18  7800a0e3  mov r0, #0x78
0011ed1c  ff0e00eb  bl 0x00122920
0011ed20  000050e3  cmp r0, #0x0
0011ed24  0300000a  beq 0x0011ed38
0011ed28  0130a0e3  mov r3, #0x1
0011ed2c  0420a0e1  cpy r2, r4
0011ed30  0510a0e1  cpy r1, r5
0011ed34  532500eb  bl 0x00128288
0011ed38  0510a0e1  cpy r1, r5
0011ed3c  000086e5  str r0, [r6,#0x0]
0011ed40  7040bde8  ldmia sp!,{r4,r5,r6,lr}
0011ed44  6c2400ea  b 0x00127efc
0011ed48  7080bde8  ldmia sp!,{r4,r5,r6,pc}
