108c84e0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c84e4  260051e3  cmp r1, #0x26
108c84e8  0050a0e1  cpy r5, r0
108c84ec  0140a0e1  cpy r4, r1
108c84f0  0300003a  bcc 0x108c8504
108c84f4  0020a0e3  mov r2, #0x0
108c84f8  0210a0e1  cpy r1, r2
108c84fc  0200a0e1  cpy r0, r2
108c8500  a3dffceb  bl 0x10800394
108c8504  040185e0  add r0, r5, r4, lsl #0x2
108c8508  1c0090e5  ldr r0, [r0,#0x1c]
108c850c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
