108a7aa0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7aa4  0040a0e1  cpy r4, r0
108a7aa8  0250a0e1  cpy r5, r2
108a7aac  0300a0e3  mov r0, #0x3
108a7ab0  990fffeb  bl 0x1086b91c
108a7ab4  050050e1  cmp r0, r5
108a7ab8  0c00001a  bne 0x108a7af0
108a7abc  2600a0e3  mov r0, #0x26
108a7ac0  950fffeb  bl 0x1086b91c
108a7ac4  000050e3  cmp r0, #0x0
108a7ac8  0800001a  bne 0x108a7af0
108a7acc  0400a0e1  cpy r0, r4
108a7ad0  0350a0e3  mov r5, #0x3
108a7ad4  b803d0e1  ldrh r0, [r0,#0x38]
108a7ad8  0510a0e1  cpy r1, r5
108a7adc  2495feeb  bl 0x1084cf74
108a7ae0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7ae4  ff1000e2  and r1, r0, #0xff
108a7ae8  2700a0e3  mov r0, #0x27
108a7aec  78f7feea  b 0x108658d4
108a7af0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
