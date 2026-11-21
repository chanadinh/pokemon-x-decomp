108adfb4  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108adfb8  0250a0e1  cpy r5, r2
108adfbc  0300a0e3  mov r0, #0x3
108adfc0  0190a0e1  cpy r9, r1
108adfc4  54f6feeb  bl 0x1086b91c
108adfc8  050050e1  cmp r0, r5
108adfcc  7000001a  bne 0x108ae194
108adfd0  0600a0e3  mov r0, #0x6
108adfd4  50f6feeb  bl 0x1086b91c
108adfd8  ff8000e2  and r8, r0, #0xff
108adfdc  0900a0e1  cpy r0, r9
108adfe0  4199feeb  bl 0x108544ec
108adfe4  000050e3  cmp r0, #0x0
108adfe8  0300001a  bne 0x108adffc
108adfec  0500a0e1  cpy r0, r5
108adff0  9918ffeb  bl 0x1087425c
108adff4  010050e3  cmp r0, #0x1
108adff8  6500000a  beq 0x108ae194
108adffc  0510a0e1  cpy r1, r5
108ae000  0900a0e1  cpy r0, r9
108ae004  2599feeb  bl 0x108544a0
108ae008  0040a0e1  cpy r4, r0
108ae00c  0810a0e1  cpy r1, r8
108ae010  0900a0e1  cpy r0, r9
108ae014  2199feeb  bl 0x108544a0
108ae018  00b0a0e1  cpy r11, r0
108ae01c  0400a0e1  cpy r0, r4
108ae020  b201d0e1  ldrh r0, [r0,#0x12]
108ae024  0060a0e1  cpy r6, r0
108ae028  0b00a0e1  cpy r0, r11
108ae02c  b201d0e1  ldrh r0, [r0,#0x12]
108ae030  0070a0e1  cpy r7, r0
108ae034  070096e1  orrs r0, r6, r7
108ae038  5500000a  beq 0x108ae194
108ae03c  0400a0e1  cpy r0, r4
108ae040  bc00d0e1  ldrh r0, [r0,#0xc]
108ae044  00a0a0e1  cpy r10, r0
108ae048  0b00a0e1  cpy r0, r11
108ae04c  bc00d0e1  ldrh r0, [r0,#0xc]
108ae050  0040a0e1  cpy r4, r0
108ae054  0600a0e1  cpy r0, r6
108ae058  9b77feeb  bl 0x1084becc
108ae05c  000050e3  cmp r0, #0x0
108ae060  00f020e3  nop
108ae064  4a00001a  bne 0x108ae194
108ae068  0700a0e1  cpy r0, r7
108ae06c  9677feeb  bl 0x1084becc
108ae070  000050e3  cmp r0, #0x0
108ae074  00f020e3  nop
108ae078  4500001a  bne 0x108ae194
108ae07c  0720a0e1  cpy r2, r7
108ae080  0a10a0e1  cpy r1, r10
108ae084  0900a0e1  cpy r0, r9
108ae088  e398feeb  bl 0x1085441c
108ae08c  000050e3  cmp r0, #0x0
108ae090  00f020e3  nop
108ae094  3e00001a  bne 0x108ae194
108ae098  0620a0e1  cpy r2, r6
108ae09c  0410a0e1  cpy r1, r4
108ae0a0  0900a0e1  cpy r0, r9
108ae0a4  dc98feeb  bl 0x1085441c
108ae0a8  000050e3  cmp r0, #0x0
108ae0ac  00f020e3  nop
108ae0b0  3700001a  bne 0x108ae194
108ae0b4  0620a0e1  cpy r2, r6
108ae0b8  0a10a0e1  cpy r1, r10
108ae0bc  0900a0e1  cpy r0, r9
108ae0c0  d598feeb  bl 0x1085441c
108ae0c4  000050e3  cmp r0, #0x0
108ae0c8  00f020e3  nop
108ae0cc  3000001a  bne 0x108ae194
108ae0d0  0720a0e1  cpy r2, r7
108ae0d4  0410a0e1  cpy r1, r4
108ae0d8  0900a0e1  cpy r0, r9
108ae0dc  ce98feeb  bl 0x1085441c
108ae0e0  000050e3  cmp r0, #0x0
108ae0e4  00f020e3  nop
108ae0e8  2900001a  bne 0x108ae194
108ae0ec  0520a0e1  cpy r2, r5
108ae0f0  2510a0e3  mov r1, #0x25
108ae0f4  0900a0e1  cpy r0, r9
108ae0f8  9feffeeb  bl 0x10869f7c
108ae0fc  0040a0e1  cpy r4, r0
108ae100  0480c0e5  strb r8, [r0,#0x4]
108ae104  8c209fe5  ldr r2, [0x108ae198]
108ae108  0210a0e3  mov r1, #0x2
108ae10c  080080e2  add r0, r0, #0x8
108ae110  6f7cfeeb  bl 0x1084d2d4
108ae114  0510a0e1  cpy r1, r5
108ae118  080084e2  add r0, r4, #0x8
108ae11c  5e7cfeeb  bl 0x1084d29c
108ae120  74a09fe5  ldr r10, [0x108ae19c]
108ae124  000057e3  cmp r7, #0x0
108ae128  0900000a  beq 0x108ae154
108ae12c  0a20a0e1  cpy r2, r10
108ae130  0210a0e3  mov r1, #0x2
108ae134  500084e2  add r0, r4, #0x50
108ae138  657cfeeb  bl 0x1084d2d4
108ae13c  0510a0e1  cpy r1, r5
108ae140  500084e2  add r0, r4, #0x50
108ae144  547cfeeb  bl 0x1084d29c
108ae148  0710a0e1  cpy r1, r7
108ae14c  500084e2  add r0, r4, #0x50
108ae150  517cfeeb  bl 0x1084d29c
108ae154  000056e3  cmp r6, #0x0
108ae158  0900000a  beq 0x108ae184
108ae15c  0a20a0e1  cpy r2, r10
108ae160  0210a0e3  mov r1, #0x2
108ae164  2c0084e2  add r0, r4, #0x2c
108ae168  597cfeeb  bl 0x1084d2d4
108ae16c  0810a0e1  cpy r1, r8
108ae170  2c0084e2  add r0, r4, #0x2c
108ae174  487cfeeb  bl 0x1084d29c
108ae178  0610a0e1  cpy r1, r6
108ae17c  2c0084e2  add r0, r4, #0x2c
108ae180  457cfeeb  bl 0x1084d29c
108ae184  0410a0e1  cpy r1, r4
108ae188  0900a0e1  cpy r0, r9
108ae18c  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108ae190  6ce6feea  b 0x10867b48
108ae194  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
