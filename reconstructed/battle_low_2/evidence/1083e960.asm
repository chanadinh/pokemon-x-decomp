1083e960  3c309fe5  ldr r3, [0x1083e9a4]
1083e964  04e02de5  str lr, [sp,#-0x4]!
1083e968  38209fe5  ldr r2, [0x1083e9a8]
1083e96c  0cd04de2  sub sp, sp, #0xc
1083e970  6cc093e5  ldr r12, [r3,#0x6c]
1083e974  02005ce1  cmp r12, r2
1083e978  6c208315  strne r2, [r3,#0x6c]
1083e97c  900190e5  ldr r0, [r0,#0x190]
1083e980  0130a0e1  cpy r3, r1
1083e984  0020a0e3  mov r2, #0x0
1083e988  b01790e5  ldr r1, [r0,#0x7b0]
1083e98c  3a0e80e2  add r0, r0, #0x3a0
1083e990  00208de5  str r2, [sp,#0x0]
1083e994  04208de5  str r2, [sp,#0x4]
1083e998  ec6500eb  bl 0x10858150
1083e99c  0cd08de2  add sp, sp, #0xc
1083e9a0  04f09de4  ldr pc, [sp],#0x4
