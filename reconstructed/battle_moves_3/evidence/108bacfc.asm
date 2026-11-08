108bacfc  0100a0e1  cpy r0, r1
108bad00  02c0a0e1  cpy r12, r2
108bad04  08402de9  stmdb sp!,{r3,lr}
108bad08  021aa0e3  mov r1, #0x2000
108bad0c  0320a0e1  cpy r2, r3
108bad10  00108de5  str r1, [sp,#0x0]
108bad14  0230a0e3  mov r3, #0x2
108bad18  0c10a0e1  cpy r1, r12
108bad1c  c140feeb  bl 0x1084b028
108bad20  0880bde8  ldmia sp!,{r3,pc}
