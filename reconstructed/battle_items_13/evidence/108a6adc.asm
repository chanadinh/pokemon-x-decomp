108a6adc  10402de9  stmdb sp!,{r4,lr}
108a6ae0  0240a0e1  cpy r4, r2
108a6ae4  0200a0e3  mov r0, #0x2
108a6ae8  8b13ffeb  bl 0x1086b91c
108a6aec  ff0000e2  and r0, r0, #0xff
108a6af0  0410a0e1  cpy r1, r4
108a6af4  ab96feeb  bl 0x1084c5a8
108a6af8  000050e3  cmp r0, #0x0
108a6afc  0300000a  beq 0x108a6b10
108a6b00  1040bde8  ldmia sp!,{r4,lr}
108a6b04  0110a0e3  mov r1, #0x1
108a6b08  5300a0e3  mov r0, #0x53
108a6b0c  70fbfeea  b 0x108658d4
108a6b10  1080bde8  ldmia sp!,{r4,pc}
