0011eb08  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0011eb0c  0050a0e1  cpy r5, r0
0011eb10  0170a0e1  cpy r7, r1
0011eb14  64109fe5  ldr r1, [0x11eb80]
0011eb18  0000a0e3  mov r0, #0x0
0011eb1c  040085e5  str r0, [r5,#0x4]
0011eb20  001085e5  str r1, [r5,#0x0]
0011eb24  0900c5e5  strb r0, [r5,#0x9]
0011eb28  0260a0e1  cpy r6, r2
0011eb2c  0201a0e1  mov r0, r2, lsl #0x2
0011eb30  0710a0e1  cpy r1, r7
0011eb34  0820c5e5  strb r2, [r5,#0x8]
0011eb38  6b0f00eb  bl 0x001228ec
0011eb3c  000056e3  cmp r6, #0x0
0011eb40  0040a0e3  mov r4, #0x0
0011eb44  040085e5  str r0, [r5,#0x4]
0011eb48  0a00009a  bls 0x0011eb78
0011eb4c  0710a0e1  cpy r1, r7
0011eb50  0c00a0e3  mov r0, #0xc
0011eb54  710f00eb  bl 0x00122920
0011eb58  000050e3  cmp r0, #0x0
0011eb5c  0710a011  cpyne r1, r7
0011eb60  7b32001b  blne 0x0012b554
0011eb64  041095e5  ldr r1, [r5,#0x4]
0011eb68  040181e7  str r0, [r1,r4,lsl #0x2]
0011eb6c  014084e2  add r4, r4, #0x1
0011eb70  060054e1  cmp r4, r6
0011eb74  f4ffff3a  bcc 0x0011eb4c
0011eb78  0500a0e1  cpy r0, r5
0011eb7c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
