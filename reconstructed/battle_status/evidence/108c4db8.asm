108c4db8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c4dbc  0150a0e1  cpy r5, r1
108c4dc0  0260a0e1  cpy r6, r2
108c4dc4  0370a0e1  cpy r7, r3
108c4dc8  0400a0e3  mov r0, #0x4
108c4dcc  d29afeeb  bl 0x1086b91c
108c4dd0  ff4000e2  and r4, r0, #0xff
108c4dd4  0600a0e1  cpy r0, r6
108c4dd8  0410a0e1  cpy r1, r4
108c4ddc  f11dfeeb  bl 0x1084c5a8
108c4de0  000050e3  cmp r0, #0x0
108c4de4  1600000a  beq 0x108c4e44
108c4de8  0410a0e1  cpy r1, r4
108c4dec  0500a0e1  cpy r0, r5
108c4df0  aa3dfeeb  bl 0x108544a0
108c4df4  0b10a0e3  mov r1, #0xb
108c4df8  d8aafeeb  bl 0x1086f960
108c4dfc  000050e3  cmp r0, #0x0
108c4e00  0f00000a  beq 0x108c4e44
108c4e04  0300a0e3  mov r0, #0x3
108c4e08  c39afeeb  bl 0x1086b91c
108c4e0c  ff0000e2  and r0, r0, #0xff
108c4e10  040050e1  cmp r0, r4
108c4e14  0a00000a  beq 0x108c4e44
108c4e18  1e00a0e3  mov r0, #0x1e
108c4e1c  be9afeeb  bl 0x1086b91c
108c4e20  7000ffe6  uxth r0, r0
108c4e24  060050e3  cmp r0, #0x6
108c4e28  0100003a  bcc 0x108c4e34
108c4e2c  0e0050e3  cmp r0, #0xe
108c4e30  0300001a  bne 0x108c4e44
108c4e34  0110a0e3  mov r1, #0x1
108c4e38  4200a0e3  mov r0, #0x42
108c4e3c  a482feeb  bl 0x108658d4
108c4e40  000087e5  str r0, [r7,#0x0]
108c4e44  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
