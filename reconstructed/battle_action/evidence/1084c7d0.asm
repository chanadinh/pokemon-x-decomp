1084c7d0  10402de9  stmdb sp!,{r4,lr}
1084c7d4  050d51e3  cmp r1, #0x140
1084c7d8  0040a0e1  cpy r4, r0
1084c7dc  0300003a  bcc 0x1084c7f0
1084c7e0  0020a0e3  mov r2, #0x0
1084c7e4  0210a0e1  cpy r1, r2
1084c7e8  0200a0e1  cpy r0, r2
1084c7ec  e8cefeeb  bl 0x10800394
1084c7f0  460c84e2  add r0, r4, #0x4600
1084c7f4  7a0080e2  add r0, r0, #0x7a
1084c7f8  1080bde8  ldmia sp!,{r4,pc}
