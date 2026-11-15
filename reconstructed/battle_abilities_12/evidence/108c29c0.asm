108c29c0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c29c4  0150a0e1  cpy r5, r1
108c29c8  0240a0e1  cpy r4, r2
108c29cc  0400a0e3  mov r0, #0x4
108c29d0  d1a3feeb  bl 0x1086b91c
108c29d4  040050e1  cmp r0, r4
108c29d8  1c00001a  bne 0x108c2a50
108c29dc  4700a0e3  mov r0, #0x47
108c29e0  cda3feeb  bl 0x1086b91c
108c29e4  000050e3  cmp r0, #0x0
108c29e8  1800001a  bne 0x108c2a50
108c29ec  0560a0e1  cpy r6, r5
108c29f0  0450a0e1  cpy r5, r4
108c29f4  0410a0e1  cpy r1, r4
108c29f8  0600a0e1  cpy r0, r6
108c29fc  a746feeb  bl 0x108544a0
108c2a00  2520feeb  bl 0x1084aa9c
108c2a04  000050e3  cmp r0, #0x0
108c2a08  1000000a  beq 0x108c2a50
108c2a0c  0520a0e1  cpy r2, r5
108c2a10  3510a0e3  mov r1, #0x35
108c2a14  0600a0e1  cpy r0, r6
108c2a18  579dfeeb  bl 0x10869f7c
108c2a1c  0040a0e1  cpy r4, r0
108c2a20  0450c0e5  strb r5, [r0,#0x4]
108c2a24  28209fe5  ldr r2, [0x108c2a54]
108c2a28  0210a0e3  mov r1, #0x2
108c2a2c  080080e2  add r0, r0, #0x8
108c2a30  272afeeb  bl 0x1084d2d4
108c2a34  0510a0e1  cpy r1, r5
108c2a38  080084e2  add r0, r4, #0x8
108c2a3c  162afeeb  bl 0x1084d29c
108c2a40  0410a0e1  cpy r1, r4
108c2a44  0600a0e1  cpy r0, r6
108c2a48  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c2a4c  3d94feea  b 0x10867b48
108c2a50  7080bde8  ldmia sp!,{r4,r5,r6,pc}
