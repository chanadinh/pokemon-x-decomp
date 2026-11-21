108acb2c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108acb30  0340a0e1  cpy r4, r3
108acb34  0180a0e1  cpy r8, r1
108acb38  000093e5  ldr r0, [r3,#0x0]
108acb3c  000050e3  cmp r0, #0x0
108acb40  1e00000a  beq 0x108acbc0
108acb44  1a00a0e3  mov r0, #0x1a
108acb48  73fbfeeb  bl 0x1086b91c
108acb4c  0060b0e1  movs r6, r0
108acb50  0200000a  beq 0x108acb60
108acb54  040094e5  ldr r0, [r4,#0x4]
108acb58  060050e1  cmp r0, r6
108acb5c  1500000a  beq 0x108acbb8
108acb60  0200a0e3  mov r0, #0x2
108acb64  6cfbfeeb  bl 0x1086b91c
108acb68  ff7000e2  and r7, r0, #0xff
108acb6c  0720a0e1  cpy r2, r7
108acb70  0310a0e3  mov r1, #0x3
108acb74  0800a0e1  cpy r0, r8
108acb78  fff4feeb  bl 0x10869f7c
108acb7c  0050a0e1  cpy r5, r0
108acb80  3c209fe5  ldr r2, [0x108acbc4]
108acb84  0210a0e3  mov r1, #0x2
108acb88  040080e2  add r0, r0, #0x4
108acb8c  d081feeb  bl 0x1084d2d4
108acb90  0710a0e1  cpy r1, r7
108acb94  040085e2  add r0, r5, #0x4
108acb98  bf81feeb  bl 0x1084d29c
108acb9c  040085e2  add r0, r5, #0x4
108acba0  00f020e3  nop
108acba4  bf4dffeb  bl 0x108802a8
108acba8  0510a0e1  cpy r1, r5
108acbac  0800a0e1  cpy r0, r8
108acbb0  e4ebfeeb  bl 0x10867b48
108acbb4  046084e5  str r6, [r4,#0x4]
108acbb8  0000a0e3  mov r0, #0x0
108acbbc  000084e5  str r0, [r4,#0x0]
108acbc0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
