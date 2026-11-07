108c0edc  38402de9  stmdb sp!,{r3,r4,r5,lr}
108c0ee0  0140a0e1  cpy r4, r1
108c0ee4  0250a0e1  cpy r5, r2
108c0ee8  0400a0e3  mov r0, #0x4
108c0eec  c37ffeeb  bl 0x10860e00
108c0ef0  1e10a0e3  mov r1, #0x1e
108c0ef4  0030a0e1  cpy r3, r0
108c0ef8  00108de5  str r1, [sp,#0x0]
108c0efc  0420a0e3  mov r2, #0x4
108c0f00  0510a0e1  cpy r1, r5
108c0f04  0400a0e1  cpy r0, r4
108c0f08  9027feeb  bl 0x1084ad50
108c0f0c  3880bde8  ldmia sp!,{r3,r4,r5,pc}
