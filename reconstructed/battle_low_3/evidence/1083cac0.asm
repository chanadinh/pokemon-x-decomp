1083cac0  74209fe5  ldr r2, [0x1083cb3c]
1083cac4  10402de9  stmdb sp!,{r4,lr}
1083cac8  0140a0e1  cpy r4, r1
1083cacc  6c109fe5  ldr r1, [0x1083cb40]
1083cad0  6c3092e5  ldr r3, [r2,#0x6c]
1083cad4  010053e1  cmp r3, r1
1083cad8  6c108215  strne r1, [r2,#0x6c]
1083cadc  0010a0e3  mov r1, #0x0
1083cae0  0010c4e5  strb r1, [r4,#0x0]
1083cae4  900190e5  ldr r0, [r0,#0x190]
1083cae8  ac9901eb  bl 0x108a31a0
1083caec  0b0050e3  cmp r0, #0xb
1083caf0  0900000a  beq 0x1083cb1c
1083caf4  050000ca  bgt 0x1083cb10
1083caf8  020050e3  cmp r0, #0x2
1083cafc  0a00000a  beq 0x1083cb2c
1083cb00  0a0050e3  cmp r0, #0xa
1083cb04  0400000a  beq 0x1083cb1c
1083cb08  0000a0e3  mov r0, #0x0
1083cb0c  1080bde8  ldmia sp!,{r4,pc}
1083cb10  0c0050e3  cmp r0, #0xc
1083cb14  0d005013  cmpne r0, #0xd
1083cb18  faffff1a  bne 0x1083cb08
1083cb1c  0a0040e2  sub r0, r0, #0xa
1083cb20  0000c4e5  strb r0, [r4,#0x0]
1083cb24  0100a0e3  mov r0, #0x1
1083cb28  1080bde8  ldmia sp!,{r4,pc}
1083cb2c  0400a0e3  mov r0, #0x4
1083cb30  0000c4e5  strb r0, [r4,#0x0]
1083cb34  0100a0e3  mov r0, #0x1
1083cb38  1080bde8  ldmia sp!,{r4,pc}
