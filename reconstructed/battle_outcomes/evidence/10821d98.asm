10821d98  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
10821d9c  0050a0e1  cpy r5, r0
10821da0  0140a0e1  cpy r4, r1
10821da4  010a80e2  add r0, r0, #0x1000
10821da8  0e10d1e5  ldrb r1, [r1,#0xe]
10821dac  830f80e2  add r0, r0, #0x20c
10821db0  452901eb  bl 0x1086c2cc
10821db4  0a0050e3  cmp r0, #0xa
10821db8  0000a003  moveq r0, #0x0
10821dbc  5f00000a  beq 0x10821f40
10821dc0  0e60d4e5  ldrb r6, [r4,#0xe]
10821dc4  087095e5  ldr r7, [r5,#0x8]
10821dc8  180056e3  cmp r6, #0x18
10821dcc  0300003a  bcc 0x10821de0
10821dd0  0020a0e3  mov r2, #0x0
10821dd4  0210a0e1  cpy r1, r2
10821dd8  0200a0e1  cpy r0, r2
10821ddc  6c79ffeb  bl 0x10800394
10821de0  066187e0  add r6, r7, r6, lsl #0x2
10821de4  840096e5  ldr r0, [r6,#0x84]
10821de8  000050e3  cmp r0, #0x0
10821dec  0300001a  bne 0x10821e00
10821df0  0020a0e3  mov r2, #0x0
10821df4  0210a0e1  cpy r1, r2
10821df8  0200a0e1  cpy r0, r2
10821dfc  6479ffeb  bl 0x10800394
10821e00  0f00d4e5  ldrb r0, [r4,#0xf]
10821e04  846096e5  ldr r6, [r6,#0x84]
10821e08  010010e3  tst r0, #0x1
10821e0c  0b00000a  beq 0x10821e40
10821e10  b420d4e1  ldrh r2, [r4,#0x4]
10821e14  0810a0e3  mov r1, #0x8
10821e18  0600a0e1  cpy r0, r6
10821e1c  0b3d01eb  bl 0x10871250
10821e20  b410d4e1  ldrh r1, [r4,#0x4]
10821e24  0c0095e5  ldr r0, [r5,#0xc]
10821e28  0e20d4e5  ldrb r2, [r4,#0xe]
10821e2c  ff1001e2  and r1, r1, #0xff
10821e30  00108de5  str r1, [sp,#0x0]
10821e34  0830a0e3  mov r3, #0x8
10821e38  1410a0e3  mov r1, #0x14
10821e3c  fa2801eb  bl 0x1086c22c
10821e40  0f00d4e5  ldrb r0, [r4,#0xf]
10821e44  020010e3  tst r0, #0x2
10821e48  0b00000a  beq 0x10821e7c
10821e4c  b620d4e1  ldrh r2, [r4,#0x6]
10821e50  0910a0e3  mov r1, #0x9
10821e54  0600a0e1  cpy r0, r6
10821e58  fc3c01eb  bl 0x10871250
10821e5c  b610d4e1  ldrh r1, [r4,#0x6]
10821e60  0c0095e5  ldr r0, [r5,#0xc]
10821e64  0e20d4e5  ldrb r2, [r4,#0xe]
10821e68  ff1001e2  and r1, r1, #0xff
10821e6c  00108de5  str r1, [sp,#0x0]
10821e70  0930a0e3  mov r3, #0x9
10821e74  1410a0e3  mov r1, #0x14
10821e78  eb2801eb  bl 0x1086c22c
10821e7c  0f00d4e5  ldrb r0, [r4,#0xf]
10821e80  040010e3  tst r0, #0x4
10821e84  0b00000a  beq 0x10821eb8
10821e88  b820d4e1  ldrh r2, [r4,#0x8]
10821e8c  0a10a0e3  mov r1, #0xa
10821e90  0600a0e1  cpy r0, r6
10821e94  ed3c01eb  bl 0x10871250
10821e98  b810d4e1  ldrh r1, [r4,#0x8]
10821e9c  0c0095e5  ldr r0, [r5,#0xc]
10821ea0  0e20d4e5  ldrb r2, [r4,#0xe]
10821ea4  ff1001e2  and r1, r1, #0xff
10821ea8  00108de5  str r1, [sp,#0x0]
10821eac  0a30a0e3  mov r3, #0xa
10821eb0  1410a0e3  mov r1, #0x14
10821eb4  dc2801eb  bl 0x1086c22c
10821eb8  0f00d4e5  ldrb r0, [r4,#0xf]
10821ebc  080010e3  tst r0, #0x8
10821ec0  0b00000a  beq 0x10821ef4
10821ec4  ba20d4e1  ldrh r2, [r4,#0xa]
10821ec8  0b10a0e3  mov r1, #0xb
10821ecc  0600a0e1  cpy r0, r6
10821ed0  de3c01eb  bl 0x10871250
10821ed4  ba10d4e1  ldrh r1, [r4,#0xa]
10821ed8  0c0095e5  ldr r0, [r5,#0xc]
10821edc  0e20d4e5  ldrb r2, [r4,#0xe]
10821ee0  ff1001e2  and r1, r1, #0xff
10821ee4  00108de5  str r1, [sp,#0x0]
10821ee8  0b30a0e3  mov r3, #0xb
10821eec  1410a0e3  mov r1, #0x14
10821ef0  cd2801eb  bl 0x1086c22c
10821ef4  0f00d4e5  ldrb r0, [r4,#0xf]
10821ef8  100010e3  tst r0, #0x10
10821efc  0b00000a  beq 0x10821f30
10821f00  bc20d4e1  ldrh r2, [r4,#0xc]
10821f04  0c10a0e3  mov r1, #0xc
10821f08  0600a0e1  cpy r0, r6
10821f0c  cf3c01eb  bl 0x10871250
10821f10  bc10d4e1  ldrh r1, [r4,#0xc]
10821f14  0c0095e5  ldr r0, [r5,#0xc]
10821f18  0e20d4e5  ldrb r2, [r4,#0xe]
10821f1c  ff1001e2  and r1, r1, #0xff
10821f20  00108de5  str r1, [sp,#0x0]
10821f24  0c30a0e3  mov r3, #0xc
10821f28  1410a0e3  mov r1, #0x14
10821f2c  be2801eb  bl 0x1086c22c
10821f30  101084e2  add r1, r4, #0x10
10821f34  0500a0e1  cpy r0, r5
10821f38  2a2701eb  bl 0x1086bbe8
10821f3c  0100a0e3  mov r0, #0x1
10821f40  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
