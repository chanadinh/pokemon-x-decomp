108abf28  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108abf2c  000051e3  cmp r1, #0x0
108abf30  10d04de2  sub sp, sp, #0x10
108abf34  2a00000a  beq 0x108abfe4
108abf38  b0809fe5  ldr r8, [0x108abff0]
108abf3c  0060a0e1  cpy r6, r0
108abf40  0150a0e1  cpy r5, r1
108abf44  0040a0e3  mov r4, #0x0
108abf48  840198e7  ldr r0, [r8,r4,lsl #0x3]
108abf4c  050050e1  cmp r0, r5
108abf50  2000001a  bne 0x108abfd8
108abf54  0c10a0e3  mov r1, #0xc
108abf58  0600a0e1  cpy r0, r6
108abf5c  1121ffeb  bl 0x108743a8
108abf60  7070ffe6  uxth r7, r0
108abf64  0600a0e1  cpy r0, r6
108abf68  1900d0e5  ldrb r0, [r0,#0x19]
108abf6c  0060a0e1  cpy r6, r0
108abf70  840188e0  add r0, r8, r4, lsl #0x3
108abf74  041090e5  ldr r1, [r0,#0x4]
108abf78  0c008de2  add r0, sp, #0xc
108abf7c  31ff2fe1  blx r1
108abf80  0010a0e1  cpy r1, r0
108abf84  0c009de5  ldr r0, [sp,#0xc]
108abf88  04108de5  str r1, [sp,#0x4]
108abf8c  0730a0e1  cpy r3, r7
108abf90  0620a0e3  mov r2, #0x6
108abf94  7000ffe6  uxth r0, r0
108abf98  08008de5  str r0, [sp,#0x8]
108abf9c  0510a0e1  cpy r1, r5
108abfa0  0500a0e3  mov r0, #0x5
108abfa4  00608de5  str r6, [sp,#0x0]
108abfa8  0484feeb  bl 0x1084cfc0
108abfac  0040b0e1  movs r4, r0
108abfb0  00f020e3  nop
108abfb4  0a00000a  beq 0x108abfe4
108abfb8  ad59ffeb  bl 0x10882674
108abfbc  0120a0e3  mov r2, #0x1
108abfc0  0610a0e3  mov r1, #0x6
108abfc4  0400a0e1  cpy r0, r4
108abfc8  ef83feeb  bl 0x1084cf8c
108abfcc  10d08de2  add sp, sp, #0x10
108abfd0  0400a0e1  cpy r0, r4
108abfd4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
108abfd8  014084e2  add r4, r4, #0x1
108abfdc  b60054e3  cmp r4, #0xb6
108abfe0  d8ffff3a  bcc 0x108abf48
108abfe4  10d08de2  add sp, sp, #0x10
108abfe8  0000a0e3  mov r0, #0x0
108abfec  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
