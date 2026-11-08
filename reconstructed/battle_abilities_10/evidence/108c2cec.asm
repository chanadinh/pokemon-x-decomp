108c2cec  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c2cf0  0150a0e1  cpy r5, r1
108c2cf4  0260a0e1  cpy r6, r2
108c2cf8  000093e5  ldr r0, [r3,#0x0]
108c2cfc  000050e3  cmp r0, #0x0
108c2d00  2a00000a  beq 0x108c2db0
108c2d04  0100a0e1  cpy r0, r1
108c2d08  5224feeb  bl 0x1084be58
108c2d0c  0040a0e1  cpy r4, r0
108c2d10  0570a0e1  cpy r7, r5
108c2d14  0610a0e1  cpy r1, r6
108c2d18  0500a0e1  cpy r0, r5
108c2d1c  df45feeb  bl 0x108544a0
108c2d20  0050a0e1  cpy r5, r0
108c2d24  bc00d0e1  ldrh r0, [r0,#0xc]
108c2d28  011c40e2  sub r1, r0, #0x100
108c2d2c  5f1051e2  subs r1, r1, #0x5f
108c2d30  1e00001a  bne 0x108c2db0
108c2d34  5301d5e5  ldrb r0, [r5,#0x153]
108c2d38  000054e3  cmp r4, #0x0
108c2d3c  0300000a  beq 0x108c2d50
108c2d40  010054e3  cmp r4, #0x1
108c2d44  02005413  cmpne r4, #0x2
108c2d48  03005413  cmpne r4, #0x3
108c2d4c  0000000a  beq 0x108c2d54
108c2d50  0040a0e3  mov r4, #0x0
108c2d54  000054e1  cmp r4, r0
108c2d58  1400000a  beq 0x108c2db0
108c2d5c  0620a0e1  cpy r2, r6
108c2d60  3a10a0e3  mov r1, #0x3a
108c2d64  0700a0e1  cpy r0, r7
108c2d68  839cfeeb  bl 0x10869f7c
108c2d6c  0050a0e1  cpy r5, r0
108c2d70  000090e5  ldr r0, [r0,#0x0]
108c2d74  0540c5e5  strb r4, [r5,#0x5]
108c2d78  0460c5e5  strb r6, [r5,#0x4]
108c2d7c  020580e3  orr r0, r0, #0x800000
108c2d80  000085e5  str r0, [r5,#0x0]
108c2d84  de20a0e3  mov r2, #0xde
108c2d88  0210a0e3  mov r1, #0x2
108c2d8c  080085e2  add r0, r5, #0x8
108c2d90  4f29feeb  bl 0x1084d2d4
108c2d94  0610a0e1  cpy r1, r6
108c2d98  080085e2  add r0, r5, #0x8
108c2d9c  3e29feeb  bl 0x1084d29c
108c2da0  0510a0e1  cpy r1, r5
108c2da4  0700a0e1  cpy r0, r7
108c2da8  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c2dac  6593feea  b 0x10867b48
108c2db0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
