108aee4c  ff5f2de9  stmdb sp!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108aee50  0050a0e1  cpy r5, r0
108aee54  0280a0e1  cpy r8, r2
108aee58  0390a0e1  cpy r9, r3
108aee5c  0300a0e3  mov r0, #0x3
108aee60  adf2feeb  bl 0x1086b91c
108aee64  080050e1  cmp r0, r8
108aee68  5e00001a  bne 0x108aefe8
108aee6c  04009de5  ldr r0, [sp,#0x4]
108aee70  0810a0e1  cpy r1, r8
108aee74  8995feeb  bl 0x108544a0
108aee78  1910a0e3  mov r1, #0x19
108aee7c  019ffeeb  bl 0x10856a88
108aee80  000050e3  cmp r0, #0x0
108aee84  18009905  ldreq r0, [r9,#0x18]
108aee88  00005003  cmpeq r0, #0x0
108aee8c  5500001a  bne 0x108aefe8
108aee90  04009de5  ldr r0, [sp,#0x4]
108aee94  0820a0e1  cpy r2, r8
108aee98  0b10a0e3  mov r1, #0xb
108aee9c  36ecfeeb  bl 0x10869f7c
108aeea0  0040a0e1  cpy r4, r0
108aeea4  1900a0e3  mov r0, #0x19
108aeea8  b400c4e1  strh r0, [r4,#0x4]
108aeeac  0500a0e1  cpy r0, r5
108aeeb0  b803d0e1  ldrh r0, [r0,#0x38]
108aeeb4  0010a0e1  cpy r1, r0
108aeeb8  0300a0e3  mov r0, #0x3
108aeebc  fa73feeb  bl 0x1084beac
108aeec0  080084e5  str r0, [r4,#0x8]
108aeec4  0000a0e3  mov r0, #0x0
108aeec8  0c00c4e5  strb r0, [r4,#0xc]
108aeecc  0f80c4e5  strb r8, [r4,#0xf]
108aeed0  04009de5  ldr r0, [sp,#0x4]
108aeed4  0410a0e1  cpy r1, r4
108aeed8  1ae3feeb  bl 0x10867b48
108aeedc  04009de5  ldr r0, [sp,#0x4]
108aeee0  0820a0e1  cpy r2, r8
108aeee4  0310a0e3  mov r1, #0x3
108aeee8  23ecfeeb  bl 0x10869f7c
108aeeec  0040a0e1  cpy r4, r0
108aeef0  f8209fe5  ldr r2, [0x108aeff0]
108aeef4  0210a0e3  mov r1, #0x2
108aeef8  040080e2  add r0, r0, #0x4
108aeefc  f478feeb  bl 0x1084d2d4
108aef00  0810a0e1  cpy r1, r8
108aef04  040084e2  add r0, r4, #0x4
108aef08  e378feeb  bl 0x1084d29c
108aef0c  04009de5  ldr r0, [sp,#0x4]
108aef10  0410a0e1  cpy r1, r4
108aef14  0be3feeb  bl 0x10867b48
108aef18  04009de5  ldr r0, [sp,#0x4]
108aef1c  0810a0e1  cpy r1, r8
108aef20  1477feeb  bl 0x1084cb78
108aef24  024b80e3  orr r4, r0, #0x800
108aef28  04009de5  ldr r0, [sp,#0x4]
108aef2c  0b10a0e3  mov r1, #0xb
108aef30  2676feeb  bl 0x1084c7d0
108aef34  0060a0e1  cpy r6, r0
108aef38  012080e2  add r2, r0, #0x1
108aef3c  04009de5  ldr r0, [sp,#0x4]
108aef40  0410a0e1  cpy r1, r4
108aef44  3573feeb  bl 0x1084bc20
108aef48  000050e3  cmp r0, #0x0
108aef4c  01a0a0e3  mov r10, #0x1
108aef50  0040a0e3  mov r4, #0x0
108aef54  02b0a083  movhi r11, #0x2
108aef58  0000c6e5  strb r0, [r6,#0x0]
108aef5c  2000009a  bls 0x108aefe4
108aef60  047086e0  add r7, r6, r4
108aef64  04009de5  ldr r0, [sp,#0x4]
108aef68  0110d7e5  ldrb r1, [r7,#0x1]
108aef6c  4b95feeb  bl 0x108544a0
108aef70  0210a0e3  mov r1, #0x2
108aef74  00f020e3  nop
108aef78  c29efeeb  bl 0x10856a88
108aef7c  000050e3  cmp r0, #0x0
108aef80  00f020e3  nop
108aef84  1200000a  beq 0x108aefd4
108aef88  04009de5  ldr r0, [sp,#0x4]
108aef8c  0820a0e1  cpy r2, r8
108aef90  0a10a0e3  mov r1, #0xa
108aef94  f8ebfeeb  bl 0x10869f7c
108aef98  04b0c0e5  strb r11, [r0,#0x4]
108aef9c  0050a0e1  cpy r5, r0
108aefa0  11a0c0e5  strb r10, [r0,#0x11]
108aefa4  0100d7e5  ldrb r0, [r7,#0x1]
108aefa8  44209fe5  ldr r2, [0x108aeff4]
108aefac  0210a0e3  mov r1, #0x2
108aefb0  0500c5e5  strb r0, [r5,#0x5]
108aefb4  140085e2  add r0, r5, #0x14
108aefb8  c578feeb  bl 0x1084d2d4
108aefbc  0110d7e5  ldrb r1, [r7,#0x1]
108aefc0  140085e2  add r0, r5, #0x14
108aefc4  b478feeb  bl 0x1084d29c
108aefc8  04009de5  ldr r0, [sp,#0x4]
108aefcc  0510a0e1  cpy r1, r5
108aefd0  dce2feeb  bl 0x10867b48
108aefd4  0000d6e5  ldrb r0, [r6,#0x0]
108aefd8  014084e2  add r4, r4, #0x1
108aefdc  040050e1  cmp r0, r4
108aefe0  deffff8a  bhi 0x108aef60
108aefe4  18a089e5  str r10, [r9,#0x18]
108aefe8  10d08de2  add sp, sp, #0x10
108aefec  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
