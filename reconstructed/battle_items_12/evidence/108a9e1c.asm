108a9e1c  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108a9e20  0260a0e1  cpy r6, r2
108a9e24  0170a0e1  cpy r7, r1
108a9e28  0090a0e1  cpy r9, r0
108a9e2c  0210a0e1  cpy r1, r2
108a9e30  28809de5  ldr r8, [sp,#0x28]
108a9e34  0700a0e1  cpy r0, r7
108a9e38  98a9feeb  bl 0x108544a0
108a9e3c  00b0a0e1  cpy r11, r0
108a9e40  0610a0e1  cpy r1, r6
108a9e44  0700a0e1  cpy r0, r7
108a9e48  94a9feeb  bl 0x108544a0
108a9e4c  0040a0e1  cpy r4, r0
108a9e50  0889feeb  bl 0x1084c278
108a9e54  0010a0e1  cpy r1, r0
108a9e58  0400a0e1  cpy r0, r4
108a9e5c  f088feeb  bl 0x1084c224
108a9e60  040050e3  cmp r0, #0x4
108a9e64  0050a0e1  cpy r5, r0
108a9e68  0400000a  beq 0x108a9e80
108a9e6c  0010a0e1  cpy r1, r0
108a9e70  0400a0e1  cpy r0, r4
108a9e74  9aa0feeb  bl 0x108520e4
108a9e78  000050e3  cmp r0, #0x0
108a9e7c  2100000a  beq 0x108a9f08
108a9e80  0700a0e1  cpy r0, r7
108a9e84  0610a0e1  cpy r1, r6
108a9e88  84a9feeb  bl 0x108544a0
108a9e8c  00a0a0e1  cpy r10, r0
108a9e90  00f020e3  nop
108a9e94  5201d0e5  ldrb r0, [r0,#0x152]
108a9e98  005050e2  subs r5, r0, #0x0
108a9e9c  0040a0e3  mov r4, #0x0
108a9ea0  0800009a  bls 0x108a9ec8
108a9ea4  ff1004e2  and r1, r4, #0xff
108a9ea8  0a00a0e1  cpy r0, r10
108a9eac  8ca0feeb  bl 0x108520e4
108a9eb0  000050e3  cmp r0, #0x0
108a9eb4  00f020e3  nop
108a9eb8  3600000a  beq 0x108a9f98
108a9ebc  014084e2  add r4, r4, #0x1
108a9ec0  040055e1  cmp r5, r4
108a9ec4  f6ffff8a  bhi 0x108a9ea4
108a9ec8  000058e3  cmp r8, #0x0
108a9ecc  3800000a  beq 0x108a9fb4
108a9ed0  0b00a0e1  cpy r0, r11
108a9ed4  5201d0e5  ldrb r0, [r0,#0x152]
108a9ed8  005050e2  subs r5, r0, #0x0
108a9edc  0040a0e3  mov r4, #0x0
108a9ee0  3300009a  bls 0x108a9fb4
108a9ee4  ff1004e2  and r1, r4, #0xff
108a9ee8  0b00a0e1  cpy r0, r11
108a9eec  a48cfeeb  bl 0x1084d184
108a9ef0  000050e3  cmp r0, #0x0
108a9ef4  00f020e3  nop
108a9ef8  2a00000a  beq 0x108a9fa8
108a9efc  ff5004e2  and r5, r4, #0xff
108a9f00  040055e3  cmp r5, #0x4
108a9f04  2a00000a  beq 0x108a9fb4
108a9f08  0620a0e1  cpy r2, r6
108a9f0c  0810a0e3  mov r1, #0x8
108a9f10  0700a0e1  cpy r0, r7
108a9f14  1800ffeb  bl 0x10869f7c
108a9f18  0710d0e5  ldrb r1, [r0,#0x7]
108a9f1c  0040a0e1  cpy r4, r0
108a9f20  0a00a0e3  mov r0, #0xa
108a9f24  0560c4e5  strb r6, [r4,#0x5]
108a9f28  0650c4e5  strb r5, [r4,#0x6]
108a9f2c  0400c4e5  strb r0, [r4,#0x4]
108a9f30  010081e3  orr r0, r1, #0x1
108a9f34  0700c4e5  strb r0, [r4,#0x7]
108a9f38  7c209fe5  ldr r2, [0x108a9fbc]
108a9f3c  0210a0e3  mov r1, #0x2
108a9f40  080084e2  add r0, r4, #0x8
108a9f44  e28cfeeb  bl 0x1084d2d4
108a9f48  0610a0e1  cpy r1, r6
108a9f4c  080084e2  add r0, r4, #0x8
108a9f50  d18cfeeb  bl 0x1084d29c
108a9f54  0900a0e1  cpy r0, r9
108a9f58  00f020e3  nop
108a9f5c  b803d0e1  ldrh r0, [r0,#0x38]
108a9f60  0010a0e1  cpy r1, r0
108a9f64  080084e2  add r0, r4, #0x8
108a9f68  cb8cfeeb  bl 0x1084d29c
108a9f6c  0510a0e1  cpy r1, r5
108a9f70  0b00a0e1  cpy r0, r11
108a9f74  3baffeeb  bl 0x10855c68
108a9f78  0010a0e1  cpy r1, r0
108a9f7c  080084e2  add r0, r4, #0x8
108a9f80  c58cfeeb  bl 0x1084d29c
108a9f84  0410a0e1  cpy r1, r4
108a9f88  0700a0e1  cpy r0, r7
108a9f8c  edf6feeb  bl 0x10867b48
108a9f90  0100a0e3  mov r0, #0x1
108a9f94  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
108a9f98  ff5004e2  and r5, r4, #0xff
108a9f9c  040055e3  cmp r5, #0x4
108a9fa0  d8ffff1a  bne 0x108a9f08
108a9fa4  c7ffffea  b 0x108a9ec8
108a9fa8  014084e2  add r4, r4, #0x1
108a9fac  050054e1  cmp r4, r5
108a9fb0  cbffff3a  bcc 0x108a9ee4
108a9fb4  0000a0e3  mov r0, #0x0
108a9fb8  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
