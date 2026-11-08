108ba1dc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba1e0  0160a0e1  cpy r6, r1
108ba1e4  0240a0e1  cpy r4, r2
108ba1e8  0300a0e3  mov r0, #0x3
108ba1ec  cac5feeb  bl 0x1086b91c
108ba1f0  040050e1  cmp r0, r4
108ba1f4  1200001a  bne 0x108ba244
108ba1f8  0400a0e3  mov r0, #0x4
108ba1fc  c6c5feeb  bl 0x1086b91c
108ba200  ff5000e2  and r5, r0, #0xff
108ba204  0510a0e1  cpy r1, r5
108ba208  0600a0e1  cpy r0, r6
108ba20c  a368feeb  bl 0x108544a0
108ba210  b201d0e1  ldrh r0, [r0,#0x12]
108ba214  000050e3  cmp r0, #0x0
108ba218  0900000a  beq 0x108ba244
108ba21c  0520a0e1  cpy r2, r5
108ba220  0410a0e1  cpy r1, r4
108ba224  0600a0e1  cpy r0, r6
108ba228  4b46feeb  bl 0x1084bb5c
108ba22c  000050e3  cmp r0, #0x0
108ba230  0300001a  bne 0x108ba244
108ba234  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba238  061ba0e3  mov r1, #0x1800
108ba23c  3200a0e3  mov r0, #0x32
108ba240  0f4bfeea  b 0x1084ce84
108ba244  7080bde8  ldmia sp!,{r4,r5,r6,pc}
