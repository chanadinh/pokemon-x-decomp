108c54dc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c54e0  0140a0e1  cpy r4, r1
108c54e4  0250a0e1  cpy r5, r2
108c54e8  0c20a0e3  mov r2, #0xc
108c54ec  0510a0e1  cpy r1, r5
108c54f0  0400a0e1  cpy r0, r4
108c54f4  ca80feeb  bl 0x10865824
108c54f8  000050e3  cmp r0, #0x0
108c54fc  0500000a  beq 0x108c5518
108c5500  0510a0e1  cpy r1, r5
108c5504  0400a0e1  cpy r0, r4
108c5508  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c550c  0130a0e3  mov r3, #0x1
108c5510  0320a0e3  mov r2, #0x3
108c5514  5b89feea  b 0x10867a88
108c5518  7080bde8  ldmia sp!,{r4,r5,r6,pc}
