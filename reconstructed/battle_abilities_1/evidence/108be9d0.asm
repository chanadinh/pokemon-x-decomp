108be9d0  38402de9  stmdb sp!,{r3,r4,r5,lr}
108be9d4  0140a0e1  cpy r4, r1
108be9d8  0250a0e1  cpy r5, r2
108be9dc  0100a0e3  mov r0, #0x1
108be9e0  0689feeb  bl 0x10860e00
108be9e4  1e10a0e3  mov r1, #0x1e
108be9e8  0030a0e1  cpy r3, r0
108be9ec  00108de5  str r1, [sp,#0x0]
108be9f0  0120a0e3  mov r2, #0x1
108be9f4  0510a0e1  cpy r1, r5
108be9f8  0400a0e1  cpy r0, r4
108be9fc  d330feeb  bl 0x1084ad50
108bea00  3880bde8  ldmia sp!,{r3,r4,r5,pc}
