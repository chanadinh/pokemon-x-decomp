0011ee68  70402de9  stmdb sp!,{r4,r5,r6,lr}
0011ee6c  0050a0e1  cpy r5, r0
0011ee70  44609fe5  ldr r6, [0x11eebc]
0011ee74  0140a0e1  cpy r4, r1
0011ee78  000096e5  ldr r0, [r6,#0x0]
0011ee7c  000050e3  cmp r0, #0x0
0011ee80  0c00001a  bne 0x0011eeb8
0011ee84  0510a0e1  cpy r1, r5
0011ee88  7800a0e3  mov r0, #0x78
0011ee8c  a30e00eb  bl 0x00122920
0011ee90  000050e3  cmp r0, #0x0
0011ee94  0300000a  beq 0x0011eea8
0011ee98  0130a0e3  mov r3, #0x1
0011ee9c  0420a0e1  cpy r2, r4
0011eea0  0510a0e1  cpy r1, r5
0011eea4  f72400eb  bl 0x00128288
0011eea8  0510a0e1  cpy r1, r5
0011eeac  000086e5  str r0, [r6,#0x0]
0011eeb0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
0011eeb4  102400ea  b 0x00127efc
0011eeb8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
