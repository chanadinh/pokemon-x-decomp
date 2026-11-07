1084d9a0  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
1084d9a4  0190a0e1  cpy r9, r1
1084d9a8  02a0a0e1  cpy r10, r2
1084d9ac  30409de5  ldr r4, [sp,#0x30]
1084d9b0  2c509de5  ldr r5, [sp,#0x2c]
1084d9b4  28609de5  ldr r6, [sp,#0x28]
1084d9b8  0a1084e2  add r1, r4, #0xa
1084d9bc  800151e1  cmp r1, r0, lsl #0x3
1084d9c0  03b0a0e1  cpy r11, r3
1084d9c4  030000da  ble 0x1084d9d8
1084d9c8  0020a0e3  mov r2, #0x0
1084d9cc  0210a0e1  cpy r1, r2
1084d9d0  0200a0e1  cpy r0, r2
1084d9d4  6ecafeeb  bl 0x10800394
1084d9d8  000056e3  cmp r6, #0x0
1084d9dc  0300001a  bne 0x1084d9f0
1084d9e0  0020a0e3  mov r2, #0x0
1084d9e4  0210a0e1  cpy r1, r2
1084d9e8  0200a0e1  cpy r0, r2
1084d9ec  68cafeeb  bl 0x10800394
1084d9f0  000055e3  cmp r5, #0x0
1084d9f4  0300001a  bne 0x1084da08
1084d9f8  0020a0e3  mov r2, #0x0
1084d9fc  0210a0e1  cpy r1, r2
1084da00  0200a0e1  cpy r0, r2
1084da04  62cafeeb  bl 0x10800394
1084da08  000054e3  cmp r4, #0x0
1084da0c  0300001a  bne 0x1084da20
1084da10  0020a0e3  mov r2, #0x0
1084da14  0210a0e1  cpy r1, r2
1084da18  0200a0e1  cpy r0, r2
1084da1c  5ccafeeb  bl 0x10800394
1084da20  0100a0e3  mov r0, #0x1
1084da24  1016a0e1  mov r1, r0, lsl r6
1084da28  1025a0e1  mov r2, r0, lsl r5
1084da2c  013041e2  sub r3, r1, #0x1
1084da30  1004a0e1  mov r0, r0, lsl r4
1084da34  011042e2  sub r1, r2, #0x1
1084da38  010040e2  sub r0, r0, #0x1
1084da3c  7360ffe6  uxth r6, r3
1084da40  7170ffe6  uxth r7, r1
1084da44  7080ffe6  uxth r8, r0
1084da48  060059e1  cmp r9, r6
1084da4c  030000da  ble 0x1084da60
1084da50  0020a0e3  mov r2, #0x0
1084da54  0210a0e1  cpy r1, r2
1084da58  0200a0e1  cpy r0, r2
1084da5c  4ccafeeb  bl 0x10800394
1084da60  07005ae1  cmp r10, r7
1084da64  030000da  ble 0x1084da78
1084da68  0020a0e3  mov r2, #0x0
1084da6c  0210a0e1  cpy r1, r2
1084da70  0200a0e1  cpy r0, r2
1084da74  46cafeeb  bl 0x10800394
1084da78  08005be1  cmp r11, r8
1084da7c  030000da  ble 0x1084da90
1084da80  0020a0e3  mov r2, #0x0
1084da84  0210a0e1  cpy r1, r2
1084da88  0200a0e1  cpy r0, r2
1084da8c  40cafeeb  bl 0x10800394
1084da90  042085e0  add r2, r5, r4
1084da94  061009e0  and r1, r9, r6
1084da98  1112a0e1  mov r1, r1, lsl r2
1084da9c  07000ae0  and r0, r10, r7
1084daa0  100481e1  orr r0, r1, r0, lsl r4
1084daa4  08200be0  and r2, r11, r8
1084daa8  020080e1  orr r0, r0, r2
1084daac  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
