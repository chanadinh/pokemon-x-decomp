108c0aec  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c0af0  0250a0e1  cpy r5, r2
108c0af4  0170a0e1  cpy r7, r1
108c0af8  0500a0e3  mov r0, #0x5
108c0afc  86abfeeb  bl 0x1086b91c
108c0b00  006050e2  subs r6, r0, #0x0
108c0b04  0040a0e3  mov r4, #0x0
108c0b08  4600009a  bls 0x108c0c28
108c0b0c  060084e2  add r0, r4, #0x6
108c0b10  ff0000e2  and r0, r0, #0xff
108c0b14  80abfeeb  bl 0x1086b91c
108c0b18  050050e1  cmp r0, r5
108c0b1c  00f020e3  nop
108c0b20  3d00001a  bne 0x108c0c1c
108c0b24  0300a0e3  mov r0, #0x3
108c0b28  7babfeeb  bl 0x1086b91c
108c0b2c  ff6000e2  and r6, r0, #0xff
108c0b30  0620a0e1  cpy r2, r6
108c0b34  0510a0e1  cpy r1, r5
108c0b38  0700a0e1  cpy r0, r7
108c0b3c  062cfeeb  bl 0x1084bb5c
108c0b40  000050e3  cmp r0, #0x0
108c0b44  00f020e3  nop
108c0b48  3600001a  bne 0x108c0c28
108c0b4c  1200a0e3  mov r0, #0x12
108c0b50  71abfeeb  bl 0x1086b91c
108c0b54  7000ffe6  uxth r0, r0
108c0b58  0010a0e3  mov r1, #0x0
108c0b5c  4efefceb  bl 0x1080049c
108c0b60  000050e3  cmp r0, #0x0
108c0b64  00f020e3  nop
108c0b68  2e00000a  beq 0x108c0c28
108c0b6c  0510a0e1  cpy r1, r5
108c0b70  0700a0e1  cpy r0, r7
108c0b74  494efeeb  bl 0x108544a0
108c0b78  0040a0e1  cpy r4, r0
108c0b7c  0610a0e1  cpy r1, r6
108c0b80  0700a0e1  cpy r0, r7
108c0b84  454efeeb  bl 0x108544a0
108c0b88  0080a0e1  cpy r8, r0
108c0b8c  0400a0e1  cpy r0, r4
108c0b90  b201d0e1  ldrh r0, [r0,#0x12]
108c0b94  000050e3  cmp r0, #0x0
108c0b98  00f020e3  nop
108c0b9c  2100001a  bne 0x108c0c28
108c0ba0  0800a0e1  cpy r0, r8
108c0ba4  b201d0e1  ldrh r0, [r0,#0x12]
108c0ba8  000050e3  cmp r0, #0x0
108c0bac  00f020e3  nop
108c0bb0  1c00000a  beq 0x108c0c28
108c0bb4  0520a0e1  cpy r2, r5
108c0bb8  2510a0e3  mov r1, #0x25
108c0bbc  0700a0e1  cpy r0, r7
108c0bc0  eda4feeb  bl 0x10869f7c
108c0bc4  0040a0e1  cpy r4, r0
108c0bc8  000090e5  ldr r0, [r0,#0x0]
108c0bcc  0460c4e5  strb r6, [r4,#0x4]
108c0bd0  732fa0e3  mov r2, #0x1cc
108c0bd4  020580e3  orr r0, r0, #0x800000
108c0bd8  000084e5  str r0, [r4,#0x0]
108c0bdc  0210a0e3  mov r1, #0x2
108c0be0  080084e2  add r0, r4, #0x8
108c0be4  ba31feeb  bl 0x1084d2d4
108c0be8  0610a0e1  cpy r1, r6
108c0bec  080084e2  add r0, r4, #0x8
108c0bf0  a931feeb  bl 0x1084d29c
108c0bf4  0800a0e1  cpy r0, r8
108c0bf8  00f020e3  nop
108c0bfc  b201d0e1  ldrh r0, [r0,#0x12]
108c0c00  0010a0e1  cpy r1, r0
108c0c04  080084e2  add r0, r4, #0x8
108c0c08  a331feeb  bl 0x1084d29c
108c0c0c  0410a0e1  cpy r1, r4
108c0c10  0700a0e1  cpy r0, r7
108c0c14  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c0c18  ca9bfeea  b 0x10867b48
108c0c1c  014084e2  add r4, r4, #0x1
108c0c20  040056e1  cmp r6, r4
108c0c24  b8ffff8a  bhi 0x108c0b0c
108c0c28  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
