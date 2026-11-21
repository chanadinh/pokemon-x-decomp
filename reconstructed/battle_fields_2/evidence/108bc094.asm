108bc094  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc098  0140a0e1  cpy r4, r1
108bc09c  1600a0e3  mov r0, #0x16
108bc0a0  1dbefeeb  bl 0x1086b91c
108bc0a4  0c0050e3  cmp r0, #0xc
108bc0a8  1000001a  bne 0x108bc0f0
108bc0ac  0300a0e3  mov r0, #0x3
108bc0b0  19befeeb  bl 0x1086b91c
108bc0b4  ff5000e2  and r5, r0, #0xff
108bc0b8  0510a0e1  cpy r1, r5
108bc0bc  0400a0e1  cpy r0, r4
108bc0c0  973ffeeb  bl 0x1084bf24
108bc0c4  000050e3  cmp r0, #0x0
108bc0c8  0800001a  bne 0x108bc0f0
108bc0cc  0510a0e1  cpy r1, r5
108bc0d0  0400a0e1  cpy r0, r4
108bc0d4  953bfeeb  bl 0x1084af30
108bc0d8  000050e3  cmp r0, #0x0
108bc0dc  0300001a  bne 0x108bc0f0
108bc0e0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bc0e4  061ba0e3  mov r1, #0x1800
108bc0e8  3200a0e3  mov r0, #0x32
108bc0ec  6443feea  b 0x1084ce84
108bc0f0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
