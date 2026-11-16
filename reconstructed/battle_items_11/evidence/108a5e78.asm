108a5e78  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a5e7c  0250a0e1  cpy r5, r2
108a5e80  0170a0e1  cpy r7, r1
108a5e84  0500a0e3  mov r0, #0x5
108a5e88  a316ffeb  bl 0x1086b91c
108a5e8c  006050e2  subs r6, r0, #0x0
108a5e90  0040a0e3  mov r4, #0x0
108a5e94  4600009a  bls 0x108a5fb4
108a5e98  060084e2  add r0, r4, #0x6
108a5e9c  ff0000e2  and r0, r0, #0xff
108a5ea0  9d16ffeb  bl 0x1086b91c
108a5ea4  050050e1  cmp r0, r5
108a5ea8  00f020e3  nop
108a5eac  3d00001a  bne 0x108a5fa8
108a5eb0  0300a0e3  mov r0, #0x3
108a5eb4  9816ffeb  bl 0x1086b91c
108a5eb8  ff6000e2  and r6, r0, #0xff
108a5ebc  0610a0e1  cpy r1, r6
108a5ec0  0700a0e1  cpy r0, r7
108a5ec4  2b9bfeeb  bl 0x1084cb78
108a5ec8  0a0050e3  cmp r0, #0xa
108a5ecc  00f020e3  nop
108a5ed0  3700000a  beq 0x108a5fb4
108a5ed4  4e00a0e3  mov r0, #0x4e
108a5ed8  8f16ffeb  bl 0x1086b91c
108a5edc  000050e3  cmp r0, #0x0
108a5ee0  00f020e3  nop
108a5ee4  3200001a  bne 0x108a5fb4
108a5ee8  0610a0e1  cpy r1, r6
108a5eec  0700a0e1  cpy r0, r7
108a5ef0  e09afeeb  bl 0x1084ca78
108a5ef4  000050e3  cmp r0, #0x0
108a5ef8  00f020e3  nop
108a5efc  2c00000a  beq 0x108a5fb4
108a5f00  0700a0e1  cpy r0, r7
108a5f04  4ffffeeb  bl 0x10865c48
108a5f08  000050e3  cmp r0, #0x0
108a5f0c  00f020e3  nop
108a5f10  2700001a  bne 0x108a5fb4
108a5f14  0520a0e1  cpy r2, r5
108a5f18  2410a0e3  mov r1, #0x24
108a5f1c  0700a0e1  cpy r0, r7
108a5f20  1510ffeb  bl 0x10869f7c
108a5f24  0040a0e1  cpy r4, r0
108a5f28  88209fe5  ldr r2, [0x108a5fb8]
108a5f2c  0210a0e3  mov r1, #0x2
108a5f30  080080e2  add r0, r0, #0x8
108a5f34  e69cfeeb  bl 0x1084d2d4
108a5f38  0510a0e1  cpy r1, r5
108a5f3c  080084e2  add r0, r4, #0x8
108a5f40  d59cfeeb  bl 0x1084d29c
108a5f44  0610a0e1  cpy r1, r6
108a5f48  080084e2  add r0, r4, #0x8
108a5f4c  d29cfeeb  bl 0x1084d29c
108a5f50  0410a0e1  cpy r1, r4
108a5f54  0700a0e1  cpy r0, r7
108a5f58  fa06ffeb  bl 0x10867b48
108a5f5c  0520a0e1  cpy r2, r5
108a5f60  2f10a0e3  mov r1, #0x2f
108a5f64  0700a0e1  cpy r0, r7
108a5f68  0310ffeb  bl 0x10869f7c
108a5f6c  0040a0e1  cpy r4, r0
108a5f70  0300a0e3  mov r0, #0x3
108a5f74  6816ffeb  bl 0x1086b91c
108a5f78  0010a0e1  cpy r1, r0
108a5f7c  0720d4e5  ldrb r2, [r4,#0x7]
108a5f80  34009fe5  ldr r0, [0x108a5fbc]
108a5f84  0610c4e5  strb r1, [r4,#0x6]
108a5f88  f010c2e3  bic r1, r2, #0xf0
108a5f8c  b400c4e1  strh r0, [r4,#0x4]
108a5f90  100081e3  orr r0, r1, #0x10
108a5f94  0700c4e5  strb r0, [r4,#0x7]
108a5f98  0410a0e1  cpy r1, r4
108a5f9c  0700a0e1  cpy r0, r7
108a5fa0  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a5fa4  e706ffea  b 0x10867b48
108a5fa8  014084e2  add r4, r4, #0x1
108a5fac  040056e1  cmp r6, r4
108a5fb0  b8ffff8a  bhi 0x108a5e98
108a5fb4  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
