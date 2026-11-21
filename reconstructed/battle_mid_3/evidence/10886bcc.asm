10886bcc  10402de9  stmdb sp!,{r4,lr}
10886bd0  10d04de2  sub sp, sp, #0x10
10886bd4  0140a0e1  cpy r4, r1
10886bd8  0120a0e3  mov r2, #0x1
10886bdc  0d10a0e1  cpy r1, sp
10886be0  b3ebfdeb  bl 0x10801ab4
10886be4  0d10a0e1  cpy r1, sp
10886be8  0400a0e1  cpy r0, r4
10886bec  75bbffeb  bl 0x108759c8
10886bf0  10d08de2  add sp, sp, #0x10
10886bf4  1080bde8  ldmia sp!,{r4,pc}
