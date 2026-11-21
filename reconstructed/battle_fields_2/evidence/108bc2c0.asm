108bc2c0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc2c4  0150a0e1  cpy r5, r1
108bc2c8  0400a0e3  mov r0, #0x4
108bc2cc  92bdfeeb  bl 0x1086b91c
108bc2d0  ff4000e2  and r4, r0, #0xff
108bc2d4  0410a0e1  cpy r1, r4
108bc2d8  0500a0e1  cpy r0, r5
108bc2dc  103ffeeb  bl 0x1084bf24
108bc2e0  000050e3  cmp r0, #0x0
108bc2e4  2d00001a  bne 0x108bc3a0
108bc2e8  0410a0e1  cpy r1, r4
108bc2ec  0500a0e1  cpy r0, r5
108bc2f0  0e3bfeeb  bl 0x1084af30
108bc2f4  000050e3  cmp r0, #0x0
108bc2f8  2800001a  bne 0x108bc3a0
108bc2fc  0400a0e3  mov r0, #0x4
108bc300  85bdfeeb  bl 0x1086b91c
108bc304  040050e1  cmp r0, r4
108bc308  0900001a  bne 0x108bc334
108bc30c  1e00a0e3  mov r0, #0x1e
108bc310  81bdfeeb  bl 0x1086b91c
108bc314  7000ffe6  uxth r0, r0
108bc318  020050e3  cmp r0, #0x2
108bc31c  0400001a  bne 0x108bc334
108bc320  0110a0e3  mov r1, #0x1
108bc324  4200a0e3  mov r0, #0x42
108bc328  69a5feeb  bl 0x108658d4
108bc32c  000050e3  cmp r0, #0x0
108bc330  0f00001a  bne 0x108bc374
108bc334  0400a0e3  mov r0, #0x4
108bc338  77bdfeeb  bl 0x1086b91c
108bc33c  040050e1  cmp r0, r4
108bc340  00f020e3  nop
108bc344  1500001a  bne 0x108bc3a0
108bc348  1e00a0e3  mov r0, #0x1e
108bc34c  72bdfeeb  bl 0x1086b91c
108bc350  7000ffe6  uxth r0, r0
108bc354  0e0050e3  cmp r0, #0xe
108bc358  1000001a  bne 0x108bc3a0
108bc35c  0110a0e3  mov r1, #0x1
108bc360  4200a0e3  mov r0, #0x42
108bc364  5aa5feeb  bl 0x108658d4
108bc368  000050e3  cmp r0, #0x0
108bc36c  00f020e3  nop
108bc370  0a00000a  beq 0x108bc3a0
108bc374  0420a0e1  cpy r2, r4
108bc378  1710a0e3  mov r1, #0x17
108bc37c  0500a0e1  cpy r0, r5
108bc380  fdb6feeb  bl 0x10869f7c
108bc384  0010a0e1  cpy r1, r0
108bc388  1000a0e3  mov r0, #0x10
108bc38c  0540c1e5  strb r4, [r1,#0x5]
108bc390  0400c1e5  strb r0, [r1,#0x4]
108bc394  0500a0e1  cpy r0, r5
108bc398  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bc39c  e9adfeea  b 0x10867b48
108bc3a0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
