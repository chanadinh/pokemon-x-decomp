0011e9dc  70402de9  stmdb sp!,{r4,r5,r6,lr}
0011e9e0  0050a0e1  cpy r5, r0
0011e9e4  44609fe5  ldr r6, [0x11ea30]
0011e9e8  0140a0e1  cpy r4, r1
0011e9ec  000096e5  ldr r0, [r6,#0x0]
0011e9f0  000050e3  cmp r0, #0x0
0011e9f4  0c00001a  bne 0x0011ea2c
0011e9f8  0510a0e1  cpy r1, r5
0011e9fc  7800a0e3  mov r0, #0x78
0011ea00  c60f00eb  bl 0x00122920
0011ea04  000050e3  cmp r0, #0x0
0011ea08  0300000a  beq 0x0011ea1c
0011ea0c  0130a0e3  mov r3, #0x1
0011ea10  0420a0e1  cpy r2, r4
0011ea14  0510a0e1  cpy r1, r5
0011ea18  1a2600eb  bl 0x00128288
0011ea1c  0510a0e1  cpy r1, r5
0011ea20  000086e5  str r0, [r6,#0x0]
0011ea24  7040bde8  ldmia sp!,{r4,r5,r6,lr}
0011ea28  332500ea  b 0x00127efc
0011ea2c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
