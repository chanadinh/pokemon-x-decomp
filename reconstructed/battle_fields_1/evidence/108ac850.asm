108ac850  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ac854  0160a0e1  cpy r6, r1
108ac858  0240a0e1  cpy r4, r2
108ac85c  0200a0e3  mov r0, #0x2
108ac860  2dfcfeeb  bl 0x1086b91c
108ac864  ff5000e2  and r5, r0, #0xff
108ac868  0500a0e1  cpy r0, r5
108ac86c  d37dfeeb  bl 0x1084bfc0
108ac870  040050e1  cmp r0, r4
108ac874  1e00001a  bne 0x108ac8f4
108ac878  0510a0e1  cpy r1, r5
108ac87c  0600a0e1  cpy r0, r6
108ac880  a77dfeeb  bl 0x1084bf24
108ac884  000050e3  cmp r0, #0x0
108ac888  1900001a  bne 0x108ac8f4
108ac88c  1f20a0e3  mov r2, #0x1f
108ac890  0d10a0e3  mov r1, #0xd
108ac894  0600a0e1  cpy r0, r6
108ac898  b7f5feeb  bl 0x10869f7c
108ac89c  1810d0e5  ldrb r1, [r0,#0x18]
108ac8a0  0040a0e1  cpy r4, r0
108ac8a4  0130a0e3  mov r3, #0x1
108ac8a8  0520a0e3  mov r2, #0x5
108ac8ac  1630c4e5  strb r3, [r4,#0x16]
108ac8b0  0420c4e5  strb r2, [r4,#0x4]
108ac8b4  ff00a0e3  mov r0, #0xff
108ac8b8  1700c4e5  strb r0, [r4,#0x17]
108ac8bc  180081e3  orr r0, r1, #0x18
108ac8c0  0c50c4e5  strb r5, [r4,#0xc]
108ac8c4  1800c4e5  strb r0, [r4,#0x18]
108ac8c8  28209fe5  ldr r2, [0x108ac8f8]
108ac8cc  0210a0e3  mov r1, #0x2
108ac8d0  1c0084e2  add r0, r4, #0x1c
108ac8d4  7e82feeb  bl 0x1084d2d4
108ac8d8  0510a0e1  cpy r1, r5
108ac8dc  1c0084e2  add r0, r4, #0x1c
108ac8e0  6d82feeb  bl 0x1084d29c
108ac8e4  0410a0e1  cpy r1, r4
108ac8e8  0600a0e1  cpy r0, r6
108ac8ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ac8f0  94ecfeea  b 0x10867b48
108ac8f4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
