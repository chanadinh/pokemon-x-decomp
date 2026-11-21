108acd0c  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
108acd10  020050e3  cmp r0, #0x2
108acd14  14d04de2  sub sp, sp, #0x14
108acd18  0060a0e1  cpy r6, r0
108acd1c  0140a0e1  cpy r4, r1
108acd20  0270a0e1  cpy r7, r2
108acd24  0300003a  bcc 0x108acd38
108acd28  0020a0e3  mov r2, #0x0
108acd2c  0210a0e1  cpy r1, r2
108acd30  0200a0e1  cpy r0, r2
108acd34  964dfdeb  bl 0x10800394
108acd38  110054e3  cmp r4, #0x11
108acd3c  0300003a  bcc 0x108acd50
108acd40  0020a0e3  mov r2, #0x0
108acd44  0210a0e1  cpy r1, r2
108acd48  0200a0e1  cpy r0, r2
108acd4c  904dfdeb  bl 0x10800394
108acd50  ec309fe5  ldr r3, [0x108ace44]
108acd54  061286e0  add r1, r6, r6, lsl #0x4
108acd58  e8209fe5  ldr r2, [0x108ace48]
108acd5c  011283e0  add r1, r3, r1, lsl #0x4
108acd60  045281e0  add r5, r1, r4, lsl #0x4
108acd64  0000a0e3  mov r0, #0x0
108acd68  0c1095e5  ldr r1, [r5,#0xc]
108acd6c  803080e0  add r3, r0, r0, lsl #0x1
108acd70  03c1d2e7  ldrb r12, [r2,r3,lsl #0x2]
108acd74  04005ce1  cmp r12, r4
108acd78  2b00001a  bne 0x108ace2c
108acd7c  000051e3  cmp r1, #0x0
108acd80  0800000a  beq 0x108acda8
108acd84  03c182e0  add r12, r2, r3, lsl #0x2
108acd88  08c09ce5  ldr r12, [r12,#0x8]
108acd8c  01005ce1  cmp r12, r1
108acd90  2500009a  bls 0x108ace2c
108acd94  000095e5  ldr r0, [r5,#0x0]
108acd98  011081e2  add r1, r1, #0x1
108acd9c  0c1085e5  str r1, [r5,#0xc]
108acda0  14d08de2  add sp, sp, #0x14
108acda4  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
108acda8  800080e0  add r0, r0, r0, lsl #0x1
108acdac  000182e0  add r0, r2, r0, lsl #0x2
108acdb0  041090e5  ldr r1, [r0,#0x4]
108acdb4  0c008de2  add r0, sp, #0xc
108acdb8  31ff2fe1  blx r1
108acdbc  0c109de5  ldr r1, [sp,#0xc]
108acdc0  00608de5  str r6, [sp,#0x0]
108acdc4  0220a0e3  mov r2, #0x2
108acdc8  0030a0e3  mov r3, #0x0
108acdcc  7110ffe6  uxth r1, r1
108acdd0  f400cde1  strd r0, r1, [sp,#0x4]
108acdd4  0410a0e1  cpy r1, r4
108acdd8  0200a0e1  cpy r0, r2
108acddc  7780feeb  bl 0x1084cfc0
108acde0  0040b0e1  movs r4, r0
108acde4  00f020e3  nop
108acde8  0300001a  bne 0x108acdfc
108acdec  0020a0e3  mov r2, #0x0
108acdf0  0210a0e1  cpy r1, r2
108acdf4  0200a0e1  cpy r0, r2
108acdf8  694dfdeb  bl 0x108003a4
108acdfc  0760a0e1  cpy r6, r7
108ace00  0720a0e1  cpy r2, r7
108ace04  0610a0e3  mov r1, #0x6
108ace08  0400a0e1  cpy r0, r4
108ace0c  5e80feeb  bl 0x1084cf8c
108ace10  0110a0e3  mov r1, #0x1
108ace14  0000a0e3  mov r0, #0x0
108ace18  f800c5e1  strd r0, r1, [r5,#0x8]
108ace1c  500085e8  stmia r5,{r4,r6}
108ace20  14d08de2  add sp, sp, #0x14
108ace24  0400a0e1  cpy r0, r4
108ace28  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
108ace2c  010080e2  add r0, r0, #0x1
108ace30  110050e3  cmp r0, #0x11
108ace34  ccffff3a  bcc 0x108acd6c
108ace38  14d08de2  add sp, sp, #0x14
108ace3c  0000a0e3  mov r0, #0x0
108ace40  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
