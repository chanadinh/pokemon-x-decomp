108c3d90  10402de9  stmdb sp!,{r4,lr}
108c3d94  0240a0e1  cpy r4, r2
108c3d98  0200a0e3  mov r0, #0x2
108c3d9c  de9efeeb  bl 0x1086b91c
108c3da0  ff1000e2  and r1, r0, #0xff
108c3da4  0400a0e1  cpy r0, r4
108c3da8  fe21feeb  bl 0x1084c5a8
108c3dac  000050e3  cmp r0, #0x0
108c3db0  0300000a  beq 0x108c3dc4
108c3db4  1040bde8  ldmia sp!,{r4,lr}
108c3db8  0110a0e3  mov r1, #0x1
108c3dbc  4200a0e3  mov r0, #0x42
108c3dc0  c386feea  b 0x108658d4
108c3dc4  1080bde8  ldmia sp!,{r4,pc}
