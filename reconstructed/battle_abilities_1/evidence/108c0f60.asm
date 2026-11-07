108c0f60  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c0f64  08d04de2  sub sp, sp, #0x8
108c0f68  0170a0e1  cpy r7, r1
108c0f6c  0250a0e1  cpy r5, r2
108c0f70  0400a0e3  mov r0, #0x4
108c0f74  68aafeeb  bl 0x1086b91c
108c0f78  050050e1  cmp r0, r5
108c0f7c  2900001a  bne 0x108c1028
108c0f80  4700a0e3  mov r0, #0x47
108c0f84  64aafeeb  bl 0x1086b91c
108c0f88  000050e3  cmp r0, #0x0
108c0f8c  2500001a  bne 0x108c1028
108c0f90  0700a0e1  cpy r0, r7
108c0f94  2b93feeb  bl 0x10865c48
108c0f98  000050e3  cmp r0, #0x0
108c0f9c  2100001a  bne 0x108c1028
108c0fa0  0510a0e1  cpy r1, r5
108c0fa4  0700a0e1  cpy r0, r7
108c0fa8  3c4dfeeb  bl 0x108544a0
108c0fac  0080a0e1  cpy r8, r0
108c0fb0  0300a0e3  mov r0, #0x3
108c0fb4  58aafeeb  bl 0x1086b91c
108c0fb8  ff1000e2  and r1, r0, #0xff
108c0fbc  0700a0e1  cpy r0, r7
108c0fc0  364dfeeb  bl 0x108544a0
108c0fc4  0060a0e1  cpy r6, r0
108c0fc8  1210a0e3  mov r1, #0x12
108c0fcc  0800a0e1  cpy r0, r8
108c0fd0  12cdfeeb  bl 0x10874420
108c0fd4  ff4000e2  and r4, r0, #0xff
108c0fd8  1210a0e3  mov r1, #0x12
108c0fdc  0600a0e1  cpy r0, r6
108c0fe0  0ecdfeeb  bl 0x10874420
108c0fe4  020054e3  cmp r4, #0x2
108c0fe8  ff0000e2  and r0, r0, #0xff
108c0fec  0d00000a  beq 0x108c1028
108c0ff0  020050e3  cmp r0, #0x2
108c0ff4  00005411  cmpne r4, r0
108c0ff8  0a00000a  beq 0x108c1028
108c0ffc  04208de2  add r2, sp, #0x4
108c1000  0810a0e1  cpy r1, r8
108c1004  0700a0e3  mov r0, #0x7
108c1008  0a2dfeeb  bl 0x1084c438
108c100c  1e00a0e3  mov r0, #0x1e
108c1010  00008de5  str r0, [sp,#0x0]
108c1014  04309de5  ldr r3, [sp,#0x4]
108c1018  0720a0e3  mov r2, #0x7
108c101c  0510a0e1  cpy r1, r5
108c1020  0700a0e1  cpy r0, r7
108c1024  4927feeb  bl 0x1084ad50
108c1028  08d08de2  add sp, sp, #0x8
108c102c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
