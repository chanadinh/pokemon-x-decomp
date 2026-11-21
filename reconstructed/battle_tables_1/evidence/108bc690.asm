108bc690  30402de9  stmdb sp!,{r4,r5,lr}
108bc694  0150a0e1  cpy r5, r1
108bc698  6c109fe5  ldr r1, [0x108bc70c]
108bc69c  0040a0e1  cpy r4, r0
108bc6a0  14d04de2  sub sp, sp, #0x14
108bc6a4  0000a0e3  mov r0, #0x0
108bc6a8  8021d1e7  ldrb r2, [r1,r0,lsl #0x3]
108bc6ac  040052e1  cmp r2, r4
108bc6b0  0f00001a  bne 0x108bc6f4
108bc6b4  800181e0  add r0, r1, r0, lsl #0x3
108bc6b8  0510a0e1  cpy r1, r5
108bc6bc  042090e5  ldr r2, [r0,#0x4]
108bc6c0  0c008de2  add r0, sp, #0xc
108bc6c4  32ff2fe1  blx r2
108bc6c8  0c109de5  ldr r1, [sp,#0xc]
108bc6cc  00508de5  str r5, [sp,#0x0]
108bc6d0  0320a0e3  mov r2, #0x3
108bc6d4  0030a0e3  mov r3, #0x0
108bc6d8  7110ffe6  uxth r1, r1
108bc6dc  f400cde1  strd r0, r1, [sp,#0x4]
108bc6e0  0410a0e1  cpy r1, r4
108bc6e4  0200a0e1  cpy r0, r2
108bc6e8  3442feeb  bl 0x1084cfc0
108bc6ec  14d08de2  add sp, sp, #0x14
108bc6f0  3080bde8  ldmia sp!,{r4,r5,pc}
108bc6f4  010080e2  add r0, r0, #0x1
108bc6f8  0b0050e3  cmp r0, #0xb
108bc6fc  e9ffff3a  bcc 0x108bc6a8
108bc700  14d08de2  add sp, sp, #0x14
108bc704  0000a0e3  mov r0, #0x0
108bc708  3080bde8  ldmia sp!,{r4,r5,pc}
