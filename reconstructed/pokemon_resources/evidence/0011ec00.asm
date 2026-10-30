0011ec00  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0011ec04  0050a0e1  cpy r5, r0
0011ec08  0170a0e1  cpy r7, r1
0011ec0c  64109fe5  ldr r1, [0x11ec78]
0011ec10  0000a0e3  mov r0, #0x0
0011ec14  040085e5  str r0, [r5,#0x4]
0011ec18  001085e5  str r1, [r5,#0x0]
0011ec1c  0c0085e5  str r0, [r5,#0xc]
0011ec20  0260a0e1  cpy r6, r2
0011ec24  0201a0e1  mov r0, r2, lsl #0x2
0011ec28  0710a0e1  cpy r1, r7
0011ec2c  082085e5  str r2, [r5,#0x8]
0011ec30  2d0f00eb  bl 0x001228ec
0011ec34  000056e3  cmp r6, #0x0
0011ec38  0040a0e3  mov r4, #0x0
0011ec3c  040085e5  str r0, [r5,#0x4]
0011ec40  0a00009a  bls 0x0011ec70
0011ec44  0710a0e1  cpy r1, r7
0011ec48  0c00a0e3  mov r0, #0xc
0011ec4c  330f00eb  bl 0x00122920
0011ec50  000050e3  cmp r0, #0x0
0011ec54  0710a011  cpyne r1, r7
0011ec58  6c32001b  blne 0x0012b610
0011ec5c  041095e5  ldr r1, [r5,#0x4]
0011ec60  040181e7  str r0, [r1,r4,lsl #0x2]
0011ec64  014084e2  add r4, r4, #0x1
0011ec68  060054e1  cmp r4, r6
0011ec6c  f4ffff3a  bcc 0x0011ec44
0011ec70  0500a0e1  cpy r0, r5
0011ec74  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
