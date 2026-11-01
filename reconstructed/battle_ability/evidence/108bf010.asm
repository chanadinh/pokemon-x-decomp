108bf010  10402de9  stmdb sp!,{r4,lr}
108bf014  0240a0e1  cpy r4, r2
108bf018  0400a0e3  mov r0, #0x4
108bf01c  3eb2feeb  bl 0x1086b91c
108bf020  040050e1  cmp r0, r4
108bf024  0900001a  bne 0x108bf050
108bf028  1600a0e3  mov r0, #0x16
108bf02c  3ab2feeb  bl 0x1086b91c
108bf030  ff0000e2  and r0, r0, #0xff
108bf034  0e0050e3  cmp r0, #0xe
108bf038  09005013  cmpne r0, #0x9
108bf03c  0300001a  bne 0x108bf050
108bf040  1040bde8  ldmia sp!,{r4,lr}
108bf044  021ba0e3  mov r1, #0x800
108bf048  3600a0e3  mov r0, #0x36
108bf04c  8c37feea  b 0x1084ce84
108bf050  1080bde8  ldmia sp!,{r4,pc}
