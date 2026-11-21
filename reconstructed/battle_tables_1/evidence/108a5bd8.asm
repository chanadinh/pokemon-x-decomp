108a5bd8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a5bdc  10d04de2  sub sp, sp, #0x10
108a5be0  0040a0e1  cpy r4, r0
108a5be4  1900d0e5  ldrb r0, [r0,#0x19]
108a5be8  0510a0e3  mov r1, #0x5
108a5bec  0c59ffeb  bl 0x1087c024
108a5bf0  000050e3  cmp r0, #0x0
108a5bf4  2000001a  bne 0x108a5c7c
108a5bf8  0400a0e1  cpy r0, r4
108a5bfc  b201d0e1  ldrh r0, [r0,#0x12]
108a5c00  000050e3  cmp r0, #0x0
108a5c04  1c00000a  beq 0x108a5c7c
108a5c08  88809fe5  ldr r8, [0x108a5c98]
108a5c0c  0460a0e1  cpy r6, r4
108a5c10  0050a0e1  cpy r5, r0
108a5c14  0040a0e3  mov r4, #0x0
108a5c18  840198e7  ldr r0, [r8,r4,lsl #0x3]
108a5c1c  050050e1  cmp r0, r5
108a5c20  1700001a  bne 0x108a5c84
108a5c24  0c10a0e3  mov r1, #0xc
108a5c28  0600a0e1  cpy r0, r6
108a5c2c  dd39ffeb  bl 0x108743a8
108a5c30  7070ffe6  uxth r7, r0
108a5c34  0600a0e1  cpy r0, r6
108a5c38  1900d0e5  ldrb r0, [r0,#0x19]
108a5c3c  0060a0e1  cpy r6, r0
108a5c40  840188e0  add r0, r8, r4, lsl #0x3
108a5c44  041090e5  ldr r1, [r0,#0x4]
108a5c48  0c008de2  add r0, sp, #0xc
108a5c4c  31ff2fe1  blx r1
108a5c50  0010a0e1  cpy r1, r0
108a5c54  0c009de5  ldr r0, [sp,#0xc]
108a5c58  04108de5  str r1, [sp,#0x4]
108a5c5c  7510ffe6  uxth r1, r5
108a5c60  0730a0e1  cpy r3, r7
108a5c64  7000ffe6  uxth r0, r0
108a5c68  08008de5  str r0, [sp,#0x8]
108a5c6c  0620a0e3  mov r2, #0x6
108a5c70  0500a0e3  mov r0, #0x5
108a5c74  00608de5  str r6, [sp,#0x0]
108a5c78  d09cfeeb  bl 0x1084cfc0
108a5c7c  10d08de2  add sp, sp, #0x10
108a5c80  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
108a5c84  014084e2  add r4, r4, #0x1
108a5c88  b60054e3  cmp r4, #0xb6
108a5c8c  e1ffff3a  bcc 0x108a5c18
108a5c90  10d08de2  add sp, sp, #0x10
108a5c94  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
