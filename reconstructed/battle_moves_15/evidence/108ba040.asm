108ba040  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba044  0150a0e1  cpy r5, r1
108ba048  0240a0e1  cpy r4, r2
108ba04c  0200a0e3  mov r0, #0x2
108ba050  31c6feeb  bl 0x1086b91c
108ba054  040050e1  cmp r0, r4
108ba058  0800001a  bne 0x108ba080
108ba05c  0420a0e1  cpy r2, r4
108ba060  1210a0e3  mov r1, #0x12
108ba064  0500a0e1  cpy r0, r5
108ba068  c3bffeeb  bl 0x10869f7c
108ba06c  0010a0e1  cpy r1, r0
108ba070  0440c0e5  strb r4, [r0,#0x4]
108ba074  0500a0e1  cpy r0, r5
108ba078  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba07c  b1b6feea  b 0x10867b48
108ba080  7080bde8  ldmia sp!,{r4,r5,r6,pc}
