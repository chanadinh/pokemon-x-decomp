1084d2f4  10402de9  stmdb sp!,{r4,lr}
1084d2f8  0140a0e1  cpy r4, r1
1084d2fc  0e10a0e3  mov r1, #0xe
1084d300  469c00eb  bl 0x10874420
1084d304  0410a0e1  cpy r1, r4
1084d308  43ccfeeb  bl 0x1080041c
1084d30c  000050e3  cmp r0, #0x0
1084d310  0100a003  moveq r0, #0x1
1084d314  1080bde8  ldmia sp!,{r4,pc}
