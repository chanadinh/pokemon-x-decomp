0011edec  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0011edf0  0050a0e1  cpy r5, r0
0011edf4  0170a0e1  cpy r7, r1
0011edf8  64109fe5  ldr r1, [0x11ee64]
0011edfc  0000a0e3  mov r0, #0x0
0011ee00  040085e5  str r0, [r5,#0x4]
0011ee04  001085e5  str r1, [r5,#0x0]
0011ee08  0900c5e5  strb r0, [r5,#0x9]
0011ee0c  0260a0e1  cpy r6, r2
0011ee10  0201a0e1  mov r0, r2, lsl #0x2
0011ee14  0710a0e1  cpy r1, r7
0011ee18  0820c5e5  strb r2, [r5,#0x8]
0011ee1c  b20e00eb  bl 0x001228ec
0011ee20  000056e3  cmp r6, #0x0
0011ee24  0040a0e3  mov r4, #0x0
0011ee28  040085e5  str r0, [r5,#0x4]
0011ee2c  0a00009a  bls 0x0011ee5c
0011ee30  0710a0e1  cpy r1, r7
0011ee34  0c00a0e3  mov r0, #0xc
0011ee38  b80e00eb  bl 0x00122920
0011ee3c  000050e3  cmp r0, #0x0
0011ee40  0710a011  cpyne r1, r7
0011ee44  c732001b  blne 0x0012b968
0011ee48  041095e5  ldr r1, [r5,#0x4]
0011ee4c  040181e7  str r0, [r1,r4,lsl #0x2]
0011ee50  014084e2  add r4, r4, #0x1
0011ee54  060054e1  cmp r4, r6
0011ee58  f4ffff3a  bcc 0x0011ee30
0011ee5c  0500a0e1  cpy r0, r5
0011ee60  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
