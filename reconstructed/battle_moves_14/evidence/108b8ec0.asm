108b8ec0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b8ec4  0150a0e1  cpy r5, r1
108b8ec8  0240a0e1  cpy r4, r2
108b8ecc  0200a0e3  mov r0, #0x2
108b8ed0  91cafeeb  bl 0x1086b91c
108b8ed4  040050e1  cmp r0, r4
108b8ed8  2500001a  bne 0x108b8f74
108b8edc  0110a0e3  mov r1, #0x1
108b8ee0  5300a0e3  mov r0, #0x53
108b8ee4  7ab2feeb  bl 0x108658d4
108b8ee8  000050e3  cmp r0, #0x0
108b8eec  2000000a  beq 0x108b8f74
108b8ef0  0560a0e1  cpy r6, r5
108b8ef4  0450a0e1  cpy r5, r4
108b8ef8  0200a0e3  mov r0, #0x2
108b8efc  86cafeeb  bl 0x1086b91c
108b8f00  050050e1  cmp r0, r5
108b8f04  1a00001a  bne 0x108b8f74
108b8f08  0510a0e1  cpy r1, r5
108b8f0c  0600a0e1  cpy r0, r6
108b8f10  626dfeeb  bl 0x108544a0
108b8f14  3a4afeeb  bl 0x1084b804
108b8f18  000050e3  cmp r0, #0x0
108b8f1c  1400001a  bne 0x108b8f74
108b8f20  0520a0e1  cpy r2, r5
108b8f24  0310a0e3  mov r1, #0x3
108b8f28  0600a0e1  cpy r0, r6
108b8f2c  12c4feeb  bl 0x10869f7c
108b8f30  0040a0e1  cpy r4, r0
108b8f34  bf2fa0e3  mov r2, #0x2fc
108b8f38  0210a0e3  mov r1, #0x2
108b8f3c  040080e2  add r0, r0, #0x4
108b8f40  e350feeb  bl 0x1084d2d4
108b8f44  0510a0e1  cpy r1, r5
108b8f48  040084e2  add r0, r4, #0x4
108b8f4c  d250feeb  bl 0x1084d29c
108b8f50  1200a0e3  mov r0, #0x12
108b8f54  70cafeeb  bl 0x1086b91c
108b8f58  0010a0e1  cpy r1, r0
108b8f5c  040084e2  add r0, r4, #0x4
108b8f60  cd50feeb  bl 0x1084d29c
108b8f64  0410a0e1  cpy r1, r4
108b8f68  0600a0e1  cpy r0, r6
108b8f6c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b8f70  f4bafeea  b 0x10867b48
108b8f74  7080bde8  ldmia sp!,{r4,r5,r6,pc}
