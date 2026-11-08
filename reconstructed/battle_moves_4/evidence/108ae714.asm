108ae714  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ae718  0150a0e1  cpy r5, r1
108ae71c  0240a0e1  cpy r4, r2
108ae720  0300a0e3  mov r0, #0x3
108ae724  7cf4feeb  bl 0x1086b91c
108ae728  040050e1  cmp r0, r4
108ae72c  0700001a  bne 0x108ae750
108ae730  0500a0e1  cpy r0, r5
108ae734  c775feeb  bl 0x1084be58
108ae738  030050e3  cmp r0, #0x3
108ae73c  0300001a  bne 0x108ae750
108ae740  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ae744  0110a0e3  mov r1, #0x1
108ae748  5300a0e3  mov r0, #0x53
108ae74c  60dcfeea  b 0x108658d4
108ae750  7080bde8  ldmia sp!,{r4,r5,r6,pc}
