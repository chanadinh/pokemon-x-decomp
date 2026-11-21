108bc0f4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc0f8  0140a0e1  cpy r4, r1
108bc0fc  1600a0e3  mov r0, #0x16
108bc100  05befeeb  bl 0x1086b91c
108bc104  0f0050e3  cmp r0, #0xf
108bc108  1000001a  bne 0x108bc150
108bc10c  0400a0e3  mov r0, #0x4
108bc110  01befeeb  bl 0x1086b91c
108bc114  ff5000e2  and r5, r0, #0xff
108bc118  0510a0e1  cpy r1, r5
108bc11c  0400a0e1  cpy r0, r4
108bc120  7f3ffeeb  bl 0x1084bf24
108bc124  000050e3  cmp r0, #0x0
108bc128  0800001a  bne 0x108bc150
108bc12c  0510a0e1  cpy r1, r5
108bc130  0400a0e1  cpy r0, r4
108bc134  7d3bfeeb  bl 0x1084af30
108bc138  000050e3  cmp r0, #0x0
108bc13c  0300001a  bne 0x108bc150
108bc140  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bc144  021ba0e3  mov r1, #0x800
108bc148  3200a0e3  mov r0, #0x32
108bc14c  4c43feea  b 0x1084ce84
108bc150  7080bde8  ldmia sp!,{r4,r5,r6,pc}
