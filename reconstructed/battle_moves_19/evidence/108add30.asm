108add30  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108add34  0250a0e1  cpy r5, r2
108add38  0170a0e1  cpy r7, r1
108add3c  0300a0e3  mov r0, #0x3
108add40  f5f6feeb  bl 0x1086b91c
108add44  050050e1  cmp r0, r5
108add48  6f00001a  bne 0x108adf0c
108add4c  0510a0e1  cpy r1, r5
108add50  0700a0e1  cpy r0, r7
108add54  d199feeb  bl 0x108544a0
108add58  0080a0e1  cpy r8, r0
108add5c  0040a0e3  mov r4, #0x0
108add60  0710a0e3  mov r1, #0x7
108add64  fd06ffeb  bl 0x1086f960
108add68  000050e3  cmp r0, #0x0
108add6c  0190a0e3  mov r9, #0x1
108add70  3400000a  beq 0x108ade48
108add74  0d10a0e3  mov r1, #0xd
108add78  0800a0e1  cpy r0, r8
108add7c  a719ffeb  bl 0x10874420
108add80  000050e3  cmp r0, #0x0
108add84  2d0000da  ble 0x108ade40
108add88  0600a0e3  mov r0, #0x6
108add8c  e2f6feeb  bl 0x1086b91c
108add90  ff6000e2  and r6, r0, #0xff
108add94  1f0056e3  cmp r6, #0x1f
108add98  2800000a  beq 0x108ade40
108add9c  0520a0e1  cpy r2, r5
108adda0  0b10a0e3  mov r1, #0xb
108adda4  0700a0e1  cpy r0, r7
108adda8  73f0feeb  bl 0x10869f7c
108addac  0040a0e1  cpy r4, r0
108addb0  0a00a0e3  mov r0, #0xa
108addb4  0f60c4e5  strb r6, [r4,#0xf]
108addb8  b400c4e1  strh r0, [r4,#0x4]
108addbc  0100a0e3  mov r0, #0x1
108addc0  080084e5  str r0, [r4,#0x8]
108addc4  44219fe5  ldr r2, [0x108adf10]
108addc8  0210a0e3  mov r1, #0x2
108addcc  140084e2  add r0, r4, #0x14
108addd0  3f7dfeeb  bl 0x1084d2d4
108addd4  0510a0e1  cpy r1, r5
108addd8  140084e2  add r0, r4, #0x14
108adddc  2e7dfeeb  bl 0x1084d29c
108adde0  0610a0e1  cpy r1, r6
108adde4  140084e2  add r0, r4, #0x14
108adde8  2b7dfeeb  bl 0x1084d29c
108addec  0410a0e1  cpy r1, r4
108addf0  0700a0e1  cpy r0, r7
108addf4  53e7feeb  bl 0x10867b48
108addf8  0520a0e1  cpy r2, r5
108addfc  0710a0e3  mov r1, #0x7
108ade00  0700a0e1  cpy r0, r7
108ade04  5cf0feeb  bl 0x10869f7c
108ade08  0040a0e1  cpy r4, r0
108ade0c  0750c0e5  strb r5, [r0,#0x7]
108ade10  0210a0e3  mov r1, #0x2
108ade14  0800a0e1  cpy r0, r8
108ade18  357dfeeb  bl 0x1084d2f4
108ade1c  001094e5  ldr r1, [r4,#0x0]
108ade20  000060e2  rsb r0, r0, #0x0
108ade24  140084e5  str r0, [r4,#0x14]
108ade28  010481e3  orr r0, r1, #0x1000000
108ade2c  0490c4e5  strb r9, [r4,#0x4]
108ade30  000084e5  str r0, [r4,#0x0]
108ade34  0410a0e1  cpy r1, r4
108ade38  0700a0e1  cpy r0, r7
108ade3c  41e7feeb  bl 0x10867b48
108ade40  0140a0e3  mov r4, #0x1
108ade44  2c0000ea  b 0x108adefc
108ade48  0520a0e1  cpy r2, r5
108ade4c  0d10a0e3  mov r1, #0xd
108ade50  0700a0e1  cpy r0, r7
108ade54  48f0feeb  bl 0x10869f7c
108ade58  1820d0e5  ldrb r2, [r0,#0x18]
108ade5c  0010a0e1  cpy r1, r0
108ade60  0530a0e3  mov r3, #0x5
108ade64  0430c1e5  strb r3, [r1,#0x4]
108ade68  ff00a0e3  mov r0, #0xff
108ade6c  1690c1e5  strb r9, [r1,#0x16]
108ade70  1700c1e5  strb r0, [r1,#0x17]
108ade74  042082e3  orr r2, r2, #0x4
108ade78  1820c1e5  strb r2, [r1,#0x18]
108ade7c  0700a0e1  cpy r0, r7
108ade80  0c50c1e5  strb r5, [r1,#0xc]
108ade84  2fe7feeb  bl 0x10867b48
108ade88  0520a0e1  cpy r2, r5
108ade8c  0d10a0e3  mov r1, #0xd
108ade90  0700a0e1  cpy r0, r7
108ade94  38f0feeb  bl 0x10869f7c
108ade98  0010a0e1  cpy r1, r0
108ade9c  1800d0e5  ldrb r0, [r0,#0x18]
108adea0  1690c1e5  strb r9, [r1,#0x16]
108adea4  0490c1e5  strb r9, [r1,#0x4]
108adea8  1790c1e5  strb r9, [r1,#0x17]
108adeac  040080e3  orr r0, r0, #0x4
108adeb0  0c50c1e5  strb r5, [r1,#0xc]
108adeb4  1800c1e5  strb r0, [r1,#0x18]
108adeb8  0700a0e1  cpy r0, r7
108adebc  21e7feeb  bl 0x10867b48
108adec0  0520a0e1  cpy r2, r5
108adec4  0d10a0e3  mov r1, #0xd
108adec8  0700a0e1  cpy r0, r7
108adecc  2af0feeb  bl 0x10869f7c
108aded0  0010a0e1  cpy r1, r0
108aded4  1800d0e5  ldrb r0, [r0,#0x18]
108aded8  0220a0e3  mov r2, #0x2
108adedc  1690c1e5  strb r9, [r1,#0x16]
108adee0  0420c1e5  strb r2, [r1,#0x4]
108adee4  1790c1e5  strb r9, [r1,#0x17]
108adee8  040080e3  orr r0, r0, #0x4
108adeec  0c50c1e5  strb r5, [r1,#0xc]
108adef0  1800c1e5  strb r0, [r1,#0x18]
108adef4  0700a0e1  cpy r0, r7
108adef8  12e7feeb  bl 0x10867b48
108adefc  0410a0e1  cpy r1, r4
108adf00  0700a0e1  cpy r0, r7
108adf04  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108adf08  fa77feea  b 0x1084bef8
108adf0c  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
