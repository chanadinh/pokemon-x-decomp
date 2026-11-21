1084bd3c  0100a0e1  cpy r0, r1
1084bd40  0210a0e1  cpy r1, r2
1084bd44  0000a0e1  cpy r0, r0
1084bd48  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1084bd4c  0060a0e1  cpy r6, r0
1084bd50  0150a0e1  cpy r5, r1
1084bd54  0300a0e3  mov r0, #0x3
1084bd58  ef7e00eb  bl 0x1086b91c
1084bd5c  050050e1  cmp r0, r5
1084bd60  3300001a  bne 0x1084be34
1084bd64  0510a0e1  cpy r1, r5
1084bd68  0600a0e1  cpy r0, r6
1084bd6c  cb2100eb  bl 0x108544a0
1084bd70  0520a0e1  cpy r2, r5
1084bd74  1510a0e3  mov r1, #0x15
1084bd78  0600a0e1  cpy r0, r6
1084bd7c  7e7800eb  bl 0x10869f7c
1084bd80  0010a0e1  cpy r1, r0
1084bd84  0700a0e3  mov r0, #0x7
1084bd88  0550c1e5  strb r5, [r1,#0x5]
1084bd8c  0400c1e5  strb r0, [r1,#0x4]
1084bd90  0600a0e1  cpy r0, r6
1084bd94  6b6f00eb  bl 0x10867b48
1084bd98  0510a0e1  cpy r1, r5
1084bd9c  0600a0e1  cpy r0, r6
1084bda0  be2100eb  bl 0x108544a0
1084bda4  0070a0e1  cpy r7, r0
1084bda8  0520a0e1  cpy r2, r5
1084bdac  2710a0e3  mov r1, #0x27
1084bdb0  0600a0e1  cpy r0, r6
1084bdb4  707800eb  bl 0x10869f7c
1084bdb8  0040a0e1  cpy r4, r0
1084bdbc  0310a0e3  mov r1, #0x3
1084bdc0  0700a0e1  cpy r0, r7
1084bdc4  58ffffeb  bl 0x1084bb2c
1084bdc8  001094e5  ldr r1, [r4,#0x0]
1084bdcc  0320a0e3  mov r2, #0x3
1084bdd0  0450c4e5  strb r5, [r4,#0x4]
1084bdd4  010080e2  add r0, r0, #0x1
1084bdd8  0520c4e5  strb r2, [r4,#0x5]
1084bddc  0600c4e5  strb r0, [r4,#0x6]
1084bde0  0114c1e3  bic r1, r1, #0x1000000
1084bde4  001084e5  str r1, [r4,#0x0]
1084bde8  0410a0e1  cpy r1, r4
1084bdec  0600a0e1  cpy r0, r6
1084bdf0  546f00eb  bl 0x10867b48
1084bdf4  0520a0e1  cpy r2, r5
1084bdf8  0310a0e3  mov r1, #0x3
1084bdfc  0600a0e1  cpy r0, r6
1084be00  5d7800eb  bl 0x10869f7c
1084be04  0040a0e1  cpy r4, r0
1084be08  28209fe5  ldr r2, [0x1084be38]
1084be0c  0210a0e3  mov r1, #0x2
1084be10  040080e2  add r0, r0, #0x4
1084be14  2e0500eb  bl 0x1084d2d4
1084be18  0510a0e1  cpy r1, r5
1084be1c  040084e2  add r0, r4, #0x4
1084be20  1d0500eb  bl 0x1084d29c
1084be24  0410a0e1  cpy r1, r4
1084be28  0600a0e1  cpy r0, r6
1084be2c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1084be30  446f00ea  b 0x10867b48
1084be34  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
