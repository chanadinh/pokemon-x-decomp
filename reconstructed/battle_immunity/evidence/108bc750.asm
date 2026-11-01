108bc750  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bc754  0160a0e1  cpy r6, r1
108bc758  0250a0e1  cpy r5, r2
108bc75c  0400a0e3  mov r0, #0x4
108bc760  6dbcfeeb  bl 0x1086b91c
108bc764  050050e1  cmp r0, r5
108bc768  2300001a  bne 0x108bc7fc
108bc76c  1200a0e3  mov r0, #0x12
108bc770  69bcfeeb  bl 0x1086b91c
108bc774  7000ffe6  uxth r0, r0
108bc778  0810a0e3  mov r1, #0x8
108bc77c  460ffdeb  bl 0x1080049c
108bc780  000050e3  cmp r0, #0x0
108bc784  1c00000a  beq 0x108bc7fc
108bc788  0110a0e3  mov r1, #0x1
108bc78c  4100a0e3  mov r0, #0x41
108bc790  4fa4feeb  bl 0x108658d4
108bc794  000050e3  cmp r0, #0x0
108bc798  1700000a  beq 0x108bc7fc
108bc79c  0520a0e1  cpy r2, r5
108bc7a0  0110a0e3  mov r1, #0x1
108bc7a4  0600a0e1  cpy r0, r6
108bc7a8  6640feeb  bl 0x1084c948
108bc7ac  0520a0e1  cpy r2, r5
108bc7b0  0310a0e3  mov r1, #0x3
108bc7b4  0600a0e1  cpy r0, r6
108bc7b8  efb5feeb  bl 0x10869f7c
108bc7bc  0040a0e1  cpy r4, r0
108bc7c0  d220a0e3  mov r2, #0xd2
108bc7c4  0210a0e3  mov r1, #0x2
108bc7c8  040080e2  add r0, r0, #0x4
108bc7cc  c042feeb  bl 0x1084d2d4
108bc7d0  0510a0e1  cpy r1, r5
108bc7d4  040084e2  add r0, r4, #0x4
108bc7d8  af42feeb  bl 0x1084d29c
108bc7dc  0410a0e1  cpy r1, r4
108bc7e0  0600a0e1  cpy r0, r6
108bc7e4  d7acfeeb  bl 0x10867b48
108bc7e8  0520a0e1  cpy r2, r5
108bc7ec  0600a0e1  cpy r0, r6
108bc7f0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bc7f4  0210a0e3  mov r1, #0x2
108bc7f8  5240feea  b 0x1084c948
108bc7fc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
