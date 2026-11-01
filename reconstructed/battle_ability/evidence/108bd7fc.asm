108bd7fc  10402de9  stmdb sp!,{r4,lr}
108bd800  0240a0e1  cpy r4, r2
108bd804  0400a0e3  mov r0, #0x4
108bd808  43b8feeb  bl 0x1086b91c
108bd80c  040050e1  cmp r0, r4
108bd810  0700001a  bne 0x108bd834
108bd814  1b00a0e3  mov r0, #0x1b
108bd818  3fb8feeb  bl 0x1086b91c
108bd81c  010050e3  cmp r0, #0x1
108bd820  0300001a  bne 0x108bd834
108bd824  1040bde8  ldmia sp!,{r4,lr}
108bd828  021aa0e3  mov r1, #0x2000
108bd82c  3600a0e3  mov r0, #0x36
108bd830  933dfeea  b 0x1084ce84
108bd834  1080bde8  ldmia sp!,{r4,pc}
