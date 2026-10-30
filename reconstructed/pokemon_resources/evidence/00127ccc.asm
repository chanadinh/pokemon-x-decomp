00127ccc  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
00127cd0  0040a0e1  cpy r4, r0
00127cd4  08d04de2  sub sp, sp, #0x8
00127cd8  0180a0e1  cpy r8, r1
00127cdc  4c0090e5  ldr r0, [r0,#0x4c]
00127ce0  0250a0e1  cpy r5, r2
00127ce4  c0a19fe5  ldr r10, [0x127eac]
00127ce8  c00fa0e1  mov r0, r0, asr #0x1f
00127cec  010090e2  adds r0, r0, #0x1
00127cf0  1100000a  beq 0x00127d3c
00127cf4  4c0094e5  ldr r0, [r4,#0x4c]
00127cf8  b413d4e1  ldrh r1, [r4,#0x34]
00127cfc  3c0094e5  ldr r0, [r4,#0x3c]
00127d00  080051e1  cmp r1, r8
00127d04  0400008a  bhi 0x00127d1c
00127d08  6c109ae5  ldr r1, [r10,#0x6c]
00127d0c  000051e3  cmp r1, #0x0
00127d10  98019f15  ldrne r0, [0x127eb0]
00127d14  31ff2f11  blxne r1
00127d18  faffffea  b 0x00127d08
00127d1c  881088e0  add r1, r8, r8, lsl #0x1
00127d20  011180e0  add r1, r0, r1, lsl #0x2
00127d24  050091e8  ldmia r1,{r0,r2}
00127d28  081091e5  ldr r1, [r1,#0x8]
00127d2c  081085e5  str r1, [r5,#0x8]
00127d30  050085e8  stmia r5,{r0,r2}
00127d34  08d08de2  add sp, sp, #0x8
00127d38  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
00127d3c  041094e5  ldr r1, [r4,#0x4]
00127d40  0400a0e1  cpy r0, r4
00127d44  fa0000eb  bl 0x00128134
00127d48  0060a0e1  cpy r6, r0
00127d4c  000090e5  ldr r0, [r0,#0x0]
00127d50  201090e5  ldr r1, [r0,#0x20]
00127d54  0600a0e1  cpy r0, r6
00127d58  31ff2fe1  blx r1
00127d5c  000056e3  cmp r6, #0x0
00127d60  0070a0e1  cpy r7, r0
00127d64  0300000a  beq 0x00127d78
00127d68  000096e5  ldr r0, [r6,#0x0]
00127d6c  041090e5  ldr r1, [r0,#0x4]
00127d70  0600a0e1  cpy r0, r6
00127d74  31ff2fe1  blx r1
00127d78  0090a0e3  mov r9, #0x0
00127d7c  04308de2  add r3, sp, #0x4
00127d80  0420a0e3  mov r2, #0x4
00127d84  0710a0e1  cpy r1, r7
00127d88  0400a0e1  cpy r0, r4
00127d8c  04908de5  str r9, [sp,#0x4]
00127d90  00908de5  str r9, [sp,#0x0]
00127d94  b0ffffeb  bl 0x00127c5c
00127d98  046094e5  ldr r6, [r4,#0x4]
00127d9c  04709de5  ldr r7, [sp,#0x4]
00127da0  000096e5  ldr r0, [r6,#0x0]
00127da4  1c1090e5  ldr r1, [r0,#0x1c]
00127da8  0600a0e1  cpy r0, r6
00127dac  31ff2fe1  blx r1
00127db0  002060e2  rsb r2, r0, #0x0
00127db4  0130a0e3  mov r3, #0x1
00127db8  0710a0e1  cpy r1, r7
00127dbc  0600a0e1  cpy r0, r6
00127dc0  c00c00eb  bl 0x0012b0c8
00127dc4  0060a0e1  cpy r6, r0
00127dc8  0030a0e1  cpy r3, r0
00127dcc  0720a0e1  cpy r2, r7
00127dd0  0010a0e3  mov r1, #0x0
00127dd4  0400a0e1  cpy r0, r4
00127dd8  00908de5  str r9, [sp,#0x0]
00127ddc  9effffeb  bl 0x00127c5c
00127de0  041094e5  ldr r1, [r4,#0x4]
00127de4  0400a0e1  cpy r0, r4
00127de8  d10000eb  bl 0x00128134
00127dec  0040a0e1  cpy r4, r0
00127df0  000090e5  ldr r0, [r0,#0x0]
00127df4  0610a0e1  cpy r1, r6
00127df8  082090e5  ldr r2, [r0,#0x8]
00127dfc  0400a0e1  cpy r0, r4
00127e00  32ff2fe1  blx r2
00127e04  000094e5  ldr r0, [r4,#0x0]
00127e08  0c1090e5  ldr r1, [r0,#0xc]
00127e0c  0400a0e1  cpy r0, r4
00127e10  31ff2fe1  blx r1
00127e14  080050e1  cmp r0, r8
00127e18  0400008a  bhi 0x00127e30
00127e1c  6c109ae5  ldr r1, [r10,#0x6c]
00127e20  000051e3  cmp r1, #0x0
00127e24  84009f15  ldrne r0, [0x127eb0]
00127e28  31ff2f11  blxne r1
00127e2c  faffffea  b 0x00127e1c
00127e30  000094e5  ldr r0, [r4,#0x0]
00127e34  4c209ae5  ldr r2, [r10,#0x4c]
00127e38  0810a0e1  cpy r1, r8
00127e3c  1c3090e5  ldr r3, [r0,#0x1c]
00127e40  0400a0e1  cpy r0, r4
00127e44  33ff2fe1  blx r3
00127e48  080085e5  str r0, [r5,#0x8]
00127e4c  000094e5  ldr r0, [r4,#0x0]
00127e50  4c209ae5  ldr r2, [r10,#0x4c]
00127e54  0810a0e1  cpy r1, r8
00127e58  103090e5  ldr r3, [r0,#0x10]
00127e5c  0400a0e1  cpy r0, r4
00127e60  33ff2fe1  blx r3
00127e64  000085e5  str r0, [r5,#0x0]
00127e68  000094e5  ldr r0, [r4,#0x0]
00127e6c  4c209ae5  ldr r2, [r10,#0x4c]
00127e70  0810a0e1  cpy r1, r8
00127e74  143090e5  ldr r3, [r0,#0x14]
00127e78  0400a0e1  cpy r0, r4
00127e7c  33ff2fe1  blx r3
00127e80  000054e3  cmp r4, #0x0
00127e84  040085e5  str r0, [r5,#0x4]
00127e88  0300000a  beq 0x00127e9c
00127e8c  000094e5  ldr r0, [r4,#0x0]
00127e90  041090e5  ldr r1, [r0,#0x4]
00127e94  0400a0e1  cpy r0, r4
00127e98  31ff2fe1  blx r1
00127e9c  08d08de2  add sp, sp, #0x8
00127ea0  0600a0e1  cpy r0, r6
00127ea4  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
00127ea8  9c5d07ea  b 0x002ff520
