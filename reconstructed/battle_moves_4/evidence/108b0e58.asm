108b0e58  10402de9  stmdb sp!,{r4,lr}
108b0e5c  0240a0e1  cpy r4, r2
108b0e60  0300a0e3  mov r0, #0x3
108b0e64  aceafeeb  bl 0x1086b91c
108b0e68  040050e1  cmp r0, r4
108b0e6c  0700001a  bne 0x108b0e90
108b0e70  2200a0e3  mov r0, #0x22
108b0e74  a8eafeeb  bl 0x1086b91c
108b0e78  030050e3  cmp r0, #0x3
108b0e7c  0300001a  bne 0x108b0e90
108b0e80  1040bde8  ldmia sp!,{r4,lr}
108b0e84  0010a0e3  mov r1, #0x0
108b0e88  4300a0e3  mov r0, #0x43
108b0e8c  90d2feea  b 0x108658d4
108b0e90  1080bde8  ldmia sp!,{r4,pc}
