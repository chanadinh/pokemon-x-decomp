108a6f60  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108a6f64  0060a0e1  cpy r6, r0
108a6f68  0180a0e1  cpy r8, r1
108a6f6c  0250a0e1  cpy r5, r2
108a6f70  0200a0e3  mov r0, #0x2
108a6f74  6812ffeb  bl 0x1086b91c
108a6f78  050050e1  cmp r0, r5
108a6f7c  5600001a  bne 0x108a70dc
108a6f80  0800a0e1  cpy r0, r8
108a6f84  2ffbfeeb  bl 0x10865c48
108a6f88  000050e3  cmp r0, #0x0
108a6f8c  5200001a  bne 0x108a70dc
108a6f90  0510a0e1  cpy r1, r5
108a6f94  0800a0e1  cpy r0, r8
108a6f98  40b5feeb  bl 0x108544a0
108a6f9c  0090a0e1  cpy r9, r0
108a6fa0  7b26ffeb  bl 0x10870994
108a6fa4  1f2000e2  and r2, r0, #0x1f
108a6fa8  030052e3  cmp r2, #0x3
108a6fac  002ba011  movne r2, r0, lsl #0x16
108a6fb0  0310a0e3  mov r1, #0x3
108a6fb4  a20d5111  cmpne r1, r2, lsr #0x1b
108a6fb8  8008a011  movne r0, r0, lsl #0x11
108a6fbc  0520a0e1  cpy r2, r5
108a6fc0  a00d5111  cmpne r1, r0, lsr #0x1b
108a6fc4  2400000a  beq 0x108a705c
108a6fc8  0610a0e3  mov r1, #0x6
108a6fcc  0800a0e1  cpy r0, r8
108a6fd0  e90bffeb  bl 0x10869f7c
108a6fd4  0040a0e1  cpy r4, r0
108a6fd8  0650c0e5  strb r5, [r0,#0x6]
108a6fdc  0810a0e3  mov r1, #0x8
108a6fe0  0900a0e1  cpy r0, r9
108a6fe4  c298feeb  bl 0x1084d2f4
108a6fe8  0010a0e1  cpy r1, r0
108a6fec  0720d4e5  ldrb r2, [r4,#0x7]
108a6ff0  e8009fe5  ldr r0, [0x108a70e0]
108a6ff4  b410c4e1  strh r1, [r4,#0x4]
108a6ff8  021082e3  orr r1, r2, #0x2
108a6ffc  b800c4e1  strh r0, [r4,#0x8]
108a7000  0710c4e5  strb r1, [r4,#0x7]
108a7004  0510a0e1  cpy r1, r5
108a7008  0800a0e1  cpy r0, r8
108a700c  5130ffeb  bl 0x10873158
108a7010  0a00c4e5  strb r0, [r4,#0xa]
108a7014  0a10a0e3  mov r1, #0xa
108a7018  0b10c4e5  strb r1, [r4,#0xb]
108a701c  c0209fe5  ldr r2, [0x108a70e4]
108a7020  0210a0e3  mov r1, #0x2
108a7024  0c0084e2  add r0, r4, #0xc
108a7028  a998feeb  bl 0x1084d2d4
108a702c  0510a0e1  cpy r1, r5
108a7030  0c0084e2  add r0, r4, #0xc
108a7034  9898feeb  bl 0x1084d29c
108a7038  0600a0e1  cpy r0, r6
108a703c  b803d0e1  ldrh r0, [r0,#0x38]
108a7040  0010a0e1  cpy r1, r0
108a7044  0c0084e2  add r0, r4, #0xc
108a7048  9398feeb  bl 0x1084d29c
108a704c  0410a0e1  cpy r1, r4
108a7050  0800a0e1  cpy r0, r8
108a7054  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108a7058  ba02ffea  b 0x10867b48
108a705c  0410a0e3  mov r1, #0x4
108a7060  0800a0e1  cpy r0, r8
108a7064  c40bffeb  bl 0x10869f7c
108a7068  0040a0e1  cpy r4, r0
108a706c  0650c0e5  strb r5, [r0,#0x6]
108a7070  0600a0e1  cpy r0, r6
108a7074  0370a0e3  mov r7, #0x3
108a7078  b803d0e1  ldrh r0, [r0,#0x38]
108a707c  0710a0e1  cpy r1, r7
108a7080  00f020e3  nop
108a7084  ba97feeb  bl 0x1084cf74
108a7088  0010a0e1  cpy r1, r0
108a708c  0900a0e1  cpy r0, r9
108a7090  9798feeb  bl 0x1084d2f4
108a7094  b400c4e1  strh r0, [r4,#0x4]
108a7098  e62fa0e3  mov r2, #0x398
108a709c  0210a0e3  mov r1, #0x2
108a70a0  080084e2  add r0, r4, #0x8
108a70a4  8a98feeb  bl 0x1084d2d4
108a70a8  0510a0e1  cpy r1, r5
108a70ac  080084e2  add r0, r4, #0x8
108a70b0  7998feeb  bl 0x1084d29c
108a70b4  0600a0e1  cpy r0, r6
108a70b8  00f020e3  nop
108a70bc  b803d0e1  ldrh r0, [r0,#0x38]
108a70c0  0010a0e1  cpy r1, r0
108a70c4  080084e2  add r0, r4, #0x8
108a70c8  7398feeb  bl 0x1084d29c
108a70cc  0410a0e1  cpy r1, r4
108a70d0  0800a0e1  cpy r0, r8
108a70d4  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108a70d8  9a02ffea  b 0x10867b48
108a70dc  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
