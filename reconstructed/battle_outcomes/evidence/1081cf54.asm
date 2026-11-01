1081cf54  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1081cf58  0050a0e1  cpy r5, r0
1081cf5c  0140a0e1  cpy r4, r1
1081cf60  010a80e2  add r0, r0, #0x1000
1081cf64  0410d1e5  ldrb r1, [r1,#0x4]
1081cf68  08d04de2  sub sp, sp, #0x8
1081cf6c  830f80e2  add r0, r0, #0x20c
1081cf70  d53c01eb  bl 0x1086c2cc
1081cf74  0a0050e3  cmp r0, #0xa
1081cf78  6400000a  beq 0x1081d110
1081cf7c  0460d4e5  ldrb r6, [r4,#0x4]
1081cf80  087095e5  ldr r7, [r5,#0x8]
1081cf84  180056e3  cmp r6, #0x18
1081cf88  0300003a  bcc 0x1081cf9c
1081cf8c  0020a0e3  mov r2, #0x0
1081cf90  0210a0e1  cpy r1, r2
1081cf94  0200a0e1  cpy r0, r2
1081cf98  fd8cffeb  bl 0x10800394
1081cf9c  066187e0  add r6, r7, r6, lsl #0x2
1081cfa0  840096e5  ldr r0, [r6,#0x84]
1081cfa4  000050e3  cmp r0, #0x0
1081cfa8  0300001a  bne 0x1081cfbc
1081cfac  0020a0e3  mov r2, #0x0
1081cfb0  0210a0e1  cpy r1, r2
1081cfb4  0200a0e1  cpy r0, r2
1081cfb8  f58cffeb  bl 0x10800394
1081cfbc  847096e5  ldr r7, [r6,#0x84]
1081cfc0  0d10a0e3  mov r1, #0xd
1081cfc4  0700a0e1  cpy r0, r7
1081cfc8  145d01eb  bl 0x10874420
1081cfcc  000050e3  cmp r0, #0x0
1081cfd0  0500d405  ldrbeq r0, [r4,#0x5]
1081cfd4  00005003  cmpeq r0, #0x0
1081cfd8  4c00000a  beq 0x1081d110
1081cfdc  000094e5  ldr r0, [r4,#0x0]
1081cfe0  1f10a0e3  mov r1, #0x1f
1081cfe4  0060a0e3  mov r6, #0x0
1081cfe8  8009a0e1  mov r0, r0, lsl #0x13
1081cfec  a00d51e1  cmp r1, r0, lsr #0x1b
1081cff0  b610d411  ldrhne r1, [r4,#0x6]
1081cff4  00005113  cmpne r1, #0x0
1081cff8  1000000a  beq 0x1081d040
1081cffc  088095e5  ldr r8, [r5,#0x8]
1081d000  a06da0e1  mov r6, r0, lsr #0x1b
1081d004  180056e3  cmp r6, #0x18
1081d008  0300003a  bcc 0x1081d01c
1081d00c  0020a0e3  mov r2, #0x0
1081d010  0210a0e1  cpy r1, r2
1081d014  0200a0e1  cpy r0, r2
1081d018  dd8cffeb  bl 0x10800394
1081d01c  066188e0  add r6, r8, r6, lsl #0x2
1081d020  840096e5  ldr r0, [r6,#0x84]
1081d024  000050e3  cmp r0, #0x0
1081d028  0300001a  bne 0x1081d03c
1081d02c  0020a0e3  mov r2, #0x0
1081d030  0210a0e1  cpy r1, r2
1081d034  0200a0e1  cpy r0, r2
1081d038  d58cffeb  bl 0x10800394
1081d03c  846096e5  ldr r6, [r6,#0x84]
1081d040  b600d4e1  ldrh r0, [r4,#0x6]
1081d044  081084e2  add r1, r4, #0x8
1081d048  00608de5  str r6, [sp,#0x0]
1081d04c  b400cde1  strh r0, [sp,#0x4]
1081d050  0500a0e1  cpy r0, r5
1081d054  e33a01eb  bl 0x1086bbe8
1081d058  000094e5  ldr r0, [r4,#0x0]
1081d05c  000056e3  cmp r6, #0x0
1081d060  0740a0e1  cpy r4, r7
1081d064  0d80a011  cpyne r8, sp
1081d068  8009a0e1  mov r0, r0, lsl #0x13
1081d06c  1970d4e5  ldrb r7, [r4,#0x19]
1081d070  a06da0e1  mov r6, r0, lsr #0x1b
1081d074  0080a003  moveq r8, #0x0
1081d078  c13a01eb  bl 0x1086bb84
1081d07c  0710a0e1  cpy r1, r7
1081d080  0400a0e3  mov r0, #0x4
1081d084  8d3a01eb  bl 0x1086bac0
1081d088  0610a0e1  cpy r1, r6
1081d08c  0300a0e3  mov r0, #0x3
1081d090  8a3a01eb  bl 0x1086bac0
1081d094  b710a0e3  mov r1, #0xb7
1081d098  0500a0e1  cpy r0, r5
1081d09c  403a01eb  bl 0x1086b9a4
1081d0a0  00f020e3  nop
1081d0a4  00f020e3  nop
1081d0a8  f43901eb  bl 0x1086b880
1081d0ac  0d10a0e3  mov r1, #0xd
1081d0b0  0400a0e1  cpy r0, r4
1081d0b4  d95c01eb  bl 0x10874420
1081d0b8  1960d4e5  ldrb r6, [r4,#0x19]
1081d0bc  0070a0e3  mov r7, #0x0
1081d0c0  b071c4e1  strh r7, [r4,#0x10]
1081d0c4  000094e5  ldr r0, [r4,#0x0]
1081d0c8  cb8cffeb  bl 0x108003fc
1081d0cc  0c0095e5  ldr r0, [r5,#0xc]
1081d0d0  0620a0e1  cpy r2, r6
1081d0d4  0410a0e3  mov r1, #0x4
1081d0d8  533c01eb  bl 0x1086c22c
1081d0dc  0c0095e5  ldr r0, [r5,#0xc]
1081d0e0  0730a0e1  cpy r3, r7
1081d0e4  0620a0e1  cpy r2, r6
1081d0e8  4b10a0e3  mov r1, #0x4b
1081d0ec  4e3c01eb  bl 0x1086c22c
1081d0f0  0830a0e1  cpy r3, r8
1081d0f4  0020a0e3  mov r2, #0x0
1081d0f8  0410a0e1  cpy r1, r4
1081d0fc  0500a0e1  cpy r0, r5
1081d100  6f1d01eb  bl 0x108646c4
1081d104  08d08de2  add sp, sp, #0x8
1081d108  0100a0e3  mov r0, #0x1
1081d10c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
1081d110  08d08de2  add sp, sp, #0x8
1081d114  0000a0e3  mov r0, #0x0
1081d118  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
