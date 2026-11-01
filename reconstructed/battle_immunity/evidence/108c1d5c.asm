108c1d5c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c1d60  0160a0e1  cpy r6, r1
108c1d64  0240a0e1  cpy r4, r2
108c1d68  0400a0e3  mov r0, #0x4
108c1d6c  eaa6feeb  bl 0x1086b91c
108c1d70  040050e1  cmp r0, r4
108c1d74  3000001a  bne 0x108c1e3c
108c1d78  0300a0e3  mov r0, #0x3
108c1d7c  e6a6feeb  bl 0x1086b91c
108c1d80  040050e1  cmp r0, r4
108c1d84  2c00000a  beq 0x108c1e3c
108c1d88  0410a0e1  cpy r1, r4
108c1d8c  0600a0e1  cpy r0, r6
108c1d90  c249feeb  bl 0x108544a0
108c1d94  1200a0e3  mov r0, #0x12
108c1d98  dfa6feeb  bl 0x1086b91c
108c1d9c  7050ffe6  uxth r5, r0
108c1da0  0500a0e1  cpy r0, r5
108c1da4  a2f9fceb  bl 0x10800434
108c1da8  000050e3  cmp r0, #0x0
108c1dac  a5005513  cmpne r5, #0xa5
108c1db0  2100000a  beq 0x108c1e3c
108c1db4  3900a0e3  mov r0, #0x39
108c1db8  d7a6feeb  bl 0x1086b91c
108c1dbc  ff0000e2  and r0, r0, #0xff
108c1dc0  070050e3  cmp r0, #0x7
108c1dc4  1c00008a  bhi 0x108c1e3c
108c1dc8  0110a0e3  mov r1, #0x1
108c1dcc  4100a0e3  mov r0, #0x41
108c1dd0  bf8efeeb  bl 0x108658d4
108c1dd4  000050e3  cmp r0, #0x0
108c1dd8  1700000a  beq 0x108c1e3c
108c1ddc  0420a0e1  cpy r2, r4
108c1de0  0110a0e3  mov r1, #0x1
108c1de4  0600a0e1  cpy r0, r6
108c1de8  d62afeeb  bl 0x1084c948
108c1dec  0420a0e1  cpy r2, r4
108c1df0  0310a0e3  mov r1, #0x3
108c1df4  0600a0e1  cpy r0, r6
108c1df8  5fa0feeb  bl 0x10869f7c
108c1dfc  0050a0e1  cpy r5, r0
108c1e00  d220a0e3  mov r2, #0xd2
108c1e04  0210a0e3  mov r1, #0x2
108c1e08  040080e2  add r0, r0, #0x4
108c1e0c  302dfeeb  bl 0x1084d2d4
108c1e10  0410a0e1  cpy r1, r4
108c1e14  040085e2  add r0, r5, #0x4
108c1e18  1f2dfeeb  bl 0x1084d29c
108c1e1c  0510a0e1  cpy r1, r5
108c1e20  0600a0e1  cpy r0, r6
108c1e24  4797feeb  bl 0x10867b48
108c1e28  0420a0e1  cpy r2, r4
108c1e2c  0600a0e1  cpy r0, r6
108c1e30  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c1e34  0210a0e3  mov r1, #0x2
108c1e38  c22afeea  b 0x1084c948
108c1e3c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
