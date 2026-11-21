108bc158  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc15c  0140a0e1  cpy r4, r1
108bc160  1600a0e3  mov r0, #0x16
108bc164  ecbdfeeb  bl 0x1086b91c
108bc168  0b0050e3  cmp r0, #0xb
108bc16c  0f00001a  bne 0x108bc1b0
108bc170  0300a0e3  mov r0, #0x3
108bc174  e8bdfeeb  bl 0x1086b91c
108bc178  ff5000e2  and r5, r0, #0xff
108bc17c  0510a0e1  cpy r1, r5
108bc180  0400a0e1  cpy r0, r4
108bc184  663ffeeb  bl 0x1084bf24
108bc188  000050e3  cmp r0, #0x0
108bc18c  0700001a  bne 0x108bc1b0
108bc190  0510a0e1  cpy r1, r5
108bc194  0400a0e1  cpy r0, r4
108bc198  643bfeeb  bl 0x1084af30
108bc19c  000050e3  cmp r0, #0x0
108bc1a0  0200001a  bne 0x108bc1b0
108bc1a4  061ba0e3  mov r1, #0x1800
108bc1a8  3200a0e3  mov r0, #0x32
108bc1ac  3443feeb  bl 0x1084ce84
108bc1b0  1200a0e3  mov r0, #0x12
108bc1b4  d8bdfeeb  bl 0x1086b91c
108bc1b8  7000ffe6  uxth r0, r0
108bc1bc  590050e3  cmp r0, #0x59
108bc1c0  de005013  cmpne r0, #0xde
108bc1c4  021c4012  subne r1, r0, #0x200
108bc1c8  0b105112  subnes r1, r1, #0xb
108bc1cc  1200001a  bne 0x108bc21c
108bc1d0  0400a0e3  mov r0, #0x4
108bc1d4  d0bdfeeb  bl 0x1086b91c
108bc1d8  ff5000e2  and r5, r0, #0xff
108bc1dc  0510a0e1  cpy r1, r5
108bc1e0  0400a0e1  cpy r0, r4
108bc1e4  4e3ffeeb  bl 0x1084bf24
108bc1e8  000050e3  cmp r0, #0x0
108bc1ec  00f020e3  nop
108bc1f0  0900001a  bne 0x108bc21c
108bc1f4  0510a0e1  cpy r1, r5
108bc1f8  0400a0e1  cpy r0, r4
108bc1fc  4b3bfeeb  bl 0x1084af30
108bc200  000050e3  cmp r0, #0x0
108bc204  00f020e3  nop
108bc208  0300001a  bne 0x108bc21c
108bc20c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bc210  021ba0e3  mov r1, #0x800
108bc214  3200a0e3  mov r0, #0x32
108bc218  1943feea  b 0x1084ce84
108bc21c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
