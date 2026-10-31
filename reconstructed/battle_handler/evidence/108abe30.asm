108abe30  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108abe34  10d04de2  sub sp, sp, #0x10
108abe38  0040a0e1  cpy r4, r0
108abe3c  b201d0e1  ldrh r0, [r0,#0x12]
108abe40  000050e3  cmp r0, #0x0
108abe44  1a00000a  beq 0x108abeb4
108abe48  84809fe5  ldr r8, [0x108abed4]
108abe4c  0460a0e1  cpy r6, r4
108abe50  0050a0e1  cpy r5, r0
108abe54  0040a0e3  mov r4, #0x0
108abe58  840198e7  ldr r0, [r8,r4,lsl #0x3]
108abe5c  050050e1  cmp r0, r5
108abe60  1500001a  bne 0x108abebc
108abe64  0c10a0e3  mov r1, #0xc
108abe68  0600a0e1  cpy r0, r6
108abe6c  4d21ffeb  bl 0x108743a8
108abe70  7070ffe6  uxth r7, r0
108abe74  0600a0e1  cpy r0, r6
108abe78  1900d0e5  ldrb r0, [r0,#0x19]
108abe7c  0060a0e1  cpy r6, r0
108abe80  840188e0  add r0, r8, r4, lsl #0x3
108abe84  041090e5  ldr r1, [r0,#0x4]
108abe88  0c008de2  add r0, sp, #0xc
108abe8c  31ff2fe1  blx r1
108abe90  0c109de5  ldr r1, [sp,#0xc]
108abe94  00608de5  str r6, [sp,#0x0]
108abe98  0730a0e1  cpy r3, r7
108abe9c  0620a0e3  mov r2, #0x6
108abea0  7110ffe6  uxth r1, r1
108abea4  f400cde1  strd r0, r1, [sp,#0x4]
108abea8  7510ffe6  uxth r1, r5
108abeac  0500a0e3  mov r0, #0x5
108abeb0  4284feeb  bl 0x1084cfc0
108abeb4  10d08de2  add sp, sp, #0x10
108abeb8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
108abebc  014084e2  add r4, r4, #0x1
108abec0  b60054e3  cmp r4, #0xb6
108abec4  e3ffff3a  bcc 0x108abe58
108abec8  10d08de2  add sp, sp, #0x10
108abecc  0000a0e3  mov r0, #0x0
108abed0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
