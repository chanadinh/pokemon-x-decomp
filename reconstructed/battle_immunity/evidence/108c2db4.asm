108c2db4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2db8  0140a0e1  cpy r4, r1
108c2dbc  0250a0e1  cpy r5, r2
108c2dc0  0c20a0e3  mov r2, #0xc
108c2dc4  0510a0e1  cpy r1, r5
108c2dc8  0400a0e1  cpy r0, r4
108c2dcc  948afeeb  bl 0x10865824
108c2dd0  000050e3  cmp r0, #0x0
108c2dd4  0400000a  beq 0x108c2dec
108c2dd8  0510a0e1  cpy r1, r5
108c2ddc  0400a0e1  cpy r0, r4
108c2de0  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c2de4  0420a0e3  mov r2, #0x4
108c2de8  2f1ffeea  b 0x1084aaac
108c2dec  7080bde8  ldmia sp!,{r4,r5,r6,pc}
