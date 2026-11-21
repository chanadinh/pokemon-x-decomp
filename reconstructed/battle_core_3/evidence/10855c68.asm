10855c68  70402de9  stmdb sp!,{r4,r5,r6,lr}
10855c6c  0050a0e1  cpy r5, r0
10855c70  5201d0e5  ldrb r0, [r0,#0x152]
10855c74  0140a0e1  cpy r4, r1
10855c78  040050e1  cmp r0, r4
10855c7c  0300008a  bhi 0x10855c90
10855c80  0020a0e3  mov r2, #0x0
10855c84  0210a0e1  cpy r1, r2
10855c88  0200a0e1  cpy r0, r2
10855c8c  c0a9feeb  bl 0x10800394
10855c90  840164e0  rsb r0, r4, r4, lsl #0x3
10855c94  800085e0  add r0, r5, r0, lsl #0x1
10855c98  010c80e2  add r0, r0, #0x100
10855c9c  bc01d0e1  ldrh r0, [r0,#0x1c]
10855ca0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
