108c3dc8  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c3dcc  0060a0e1  cpy r6, r0
108c3dd0  0150a0e1  cpy r5, r1
108c3dd4  0240a0e1  cpy r4, r2
108c3dd8  0200a0e3  mov r0, #0x2
108c3ddc  ce9efeeb  bl 0x1086b91c
108c3de0  040050e1  cmp r0, r4
108c3de4  2600001a  bne 0x108c3e84
108c3de8  1000a0e3  mov r0, #0x10
108c3dec  ca9efeeb  bl 0x1086b91c
108c3df0  0070a0e1  cpy r7, r0
108c3df4  0600a0e1  cpy r0, r6
108c3df8  b803d0e1  ldrh r0, [r0,#0x38]
108c3dfc  000057e1  cmp r7, r0
108c3e00  1f00000a  beq 0x108c3e84
108c3e04  0560a0e1  cpy r6, r5
108c3e08  0450a0e1  cpy r5, r4
108c3e0c  0410a0e1  cpy r1, r4
108c3e10  0600a0e1  cpy r0, r6
108c3e14  a141feeb  bl 0x108544a0
108c3e18  0040a0e1  cpy r4, r0
108c3e1c  bc00d0e1  ldrh r0, [r0,#0xc]
108c3e20  011c40e2  sub r1, r0, #0x100
108c3e24  5f1051e2  subs r1, r1, #0x5f
108c3e28  1500001a  bne 0x108c3e84
108c3e2c  5301d4e5  ldrb r0, [r4,#0x153]
108c3e30  000050e3  cmp r0, #0x0
108c3e34  1200000a  beq 0x108c3e84
108c3e38  0520a0e1  cpy r2, r5
108c3e3c  3a10a0e3  mov r1, #0x3a
108c3e40  0600a0e1  cpy r0, r6
108c3e44  4c98feeb  bl 0x10869f7c
108c3e48  0040a0e1  cpy r4, r0
108c3e4c  0000a0e3  mov r0, #0x0
108c3e50  0450c4e5  strb r5, [r4,#0x4]
108c3e54  0500c4e5  strb r0, [r4,#0x5]
108c3e58  de20a0e3  mov r2, #0xde
108c3e5c  0210a0e3  mov r1, #0x2
108c3e60  080084e2  add r0, r4, #0x8
108c3e64  1a25feeb  bl 0x1084d2d4
108c3e68  0510a0e1  cpy r1, r5
108c3e6c  080084e2  add r0, r4, #0x8
108c3e70  0925feeb  bl 0x1084d29c
108c3e74  0410a0e1  cpy r1, r4
108c3e78  0600a0e1  cpy r0, r6
108c3e7c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c3e80  308ffeea  b 0x10867b48
108c3e84  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
