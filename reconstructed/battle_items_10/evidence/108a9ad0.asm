108a9ad0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a9ad4  0060a0e1  cpy r6, r0
108a9ad8  0140a0e1  cpy r4, r1
108a9adc  0250a0e1  cpy r5, r2
108a9ae0  0c00a0e3  mov r0, #0xc
108a9ae4  8c07ffeb  bl 0x1086b91c
108a9ae8  060050e3  cmp r0, #0x6
108a9aec  0c00000a  beq 0x108a9b24
108a9af0  0510a0e1  cpy r1, r5
108a9af4  0400a0e1  cpy r0, r4
108a9af8  68aafeeb  bl 0x108544a0
108a9afc  8f89feeb  bl 0x1084c140
108a9b00  000050e3  cmp r0, #0x0
108a9b04  0600000a  beq 0x108a9b24
108a9b08  0600a0e1  cpy r0, r6
108a9b0c  968bfeeb  bl 0x1084c96c
108a9b10  0520a0e1  cpy r2, r5
108a9b14  0400a0e1  cpy r0, r4
108a9b18  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a9b1c  0010a0e3  mov r1, #0x0
108a9b20  888bfeea  b 0x1084c948
108a9b24  7080bde8  ldmia sp!,{r4,r5,r6,pc}
