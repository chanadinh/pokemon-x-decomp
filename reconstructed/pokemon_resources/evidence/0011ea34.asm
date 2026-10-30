0011ea34  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
0011ea38  0050a0e1  cpy r5, r0
0011ea3c  0170a0e1  cpy r7, r1
0011ea40  64109fe5  ldr r1, [0x11eaac]
0011ea44  0000a0e3  mov r0, #0x0
0011ea48  040085e5  str r0, [r5,#0x4]
0011ea4c  001085e5  str r1, [r5,#0x0]
0011ea50  0820c5e5  strb r2, [r5,#0x8]
0011ea54  0900c5e5  strb r0, [r5,#0x9]
0011ea58  0260a0e1  cpy r6, r2
0011ea5c  0201a0e1  mov r0, r2, lsl #0x2
0011ea60  0710a0e1  cpy r1, r7
0011ea64  a00f00eb  bl 0x001228ec
0011ea68  000056e3  cmp r6, #0x0
0011ea6c  0040a0e3  mov r4, #0x0
0011ea70  040085e5  str r0, [r5,#0x4]
0011ea74  0a00009a  bls 0x0011eaa4
0011ea78  0710a0e1  cpy r1, r7
0011ea7c  670fa0e3  mov r0, #0x19c
0011ea80  a60f00eb  bl 0x00122920
0011ea84  000050e3  cmp r0, #0x0
0011ea88  00f020e3  nop
0011ea8c  0433001b  blne 0x0012b6a4
0011ea90  041095e5  ldr r1, [r5,#0x4]
0011ea94  040181e7  str r0, [r1,r4,lsl #0x2]
0011ea98  014084e2  add r4, r4, #0x1
0011ea9c  060054e1  cmp r4, r6
0011eaa0  f4ffff3a  bcc 0x0011ea78
0011eaa4  0500a0e1  cpy r0, r5
0011eaa8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
