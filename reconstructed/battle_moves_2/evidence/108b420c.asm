108b420c  10402de9  stmdb sp!,{r4,lr}
108b4210  0240a0e1  cpy r4, r2
108b4214  0300a0e3  mov r0, #0x3
108b4218  bfddfeeb  bl 0x1086b91c
108b421c  040050e1  cmp r0, r4
108b4220  0900001a  bne 0x108b424c
108b4224  3d00a0e3  mov r0, #0x3d
108b4228  bbddfeeb  bl 0x1086b91c
108b422c  0b0050e3  cmp r0, #0xb
108b4230  0500001a  bne 0x108b424c
108b4234  3e00a0e3  mov r0, #0x3e
108b4238  b7ddfeeb  bl 0x1086b91c
108b423c  011080e2  add r1, r0, #0x1
108b4240  1040bde8  ldmia sp!,{r4,lr}
108b4244  3e00a0e3  mov r0, #0x3e
108b4248  a1c5feea  b 0x108658d4
108b424c  1080bde8  ldmia sp!,{r4,pc}
