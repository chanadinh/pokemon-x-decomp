108b8318  10402de9  stmdb sp!,{r4,lr}
108b831c  0040a0e1  cpy r4, r0
108b8320  0200a0e3  mov r0, #0x2
108b8324  7ccdfeeb  bl 0x1086b91c
108b8328  1f0050e3  cmp r0, #0x1f
108b832c  0200001a  bne 0x108b833c
108b8330  0400a0e1  cpy r0, r4
108b8334  1040bde8  ldmia sp!,{r4,lr}
108b8338  a353feea  b 0x1084d1cc
108b833c  1080bde8  ldmia sp!,{r4,pc}
