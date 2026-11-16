108ba248  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba24c  0150a0e1  cpy r5, r1
108ba250  0240a0e1  cpy r4, r2
108ba254  0300a0e3  mov r0, #0x3
108ba258  afc5feeb  bl 0x1086b91c
108ba25c  040050e1  cmp r0, r4
108ba260  0d00001a  bne 0x108ba29c
108ba264  0110a0e3  mov r1, #0x1
108ba268  5300a0e3  mov r0, #0x53
108ba26c  98adfeeb  bl 0x108658d4
108ba270  000050e3  cmp r0, #0x0
108ba274  0800000a  beq 0x108ba29c
108ba278  0420a0e1  cpy r2, r4
108ba27c  1210a0e3  mov r1, #0x12
108ba280  0500a0e1  cpy r0, r5
108ba284  3cbffeeb  bl 0x10869f7c
108ba288  0010a0e1  cpy r1, r0
108ba28c  0440c0e5  strb r4, [r0,#0x4]
108ba290  0500a0e1  cpy r0, r5
108ba294  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba298  2ab6feea  b 0x10867b48
108ba29c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
