108c3f58  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108c3f5c  0160a0e1  cpy r6, r1
108c3f60  0240a0e1  cpy r4, r2
108c3f64  70709fe5  ldr r7, [0x108c3fdc]
108c3f68  0200a0e3  mov r0, #0x2
108c3f6c  6a9efeeb  bl 0x1086b91c
108c3f70  040050e1  cmp r0, r4
108c3f74  1700001a  bne 0x108c3fd8
108c3f78  0420a0e1  cpy r2, r4
108c3f7c  0110a0e3  mov r1, #0x1
108c3f80  0600a0e1  cpy r0, r6
108c3f84  6f22feeb  bl 0x1084c948
108c3f88  0420a0e1  cpy r2, r4
108c3f8c  0310a0e3  mov r1, #0x3
108c3f90  0600a0e1  cpy r0, r6
108c3f94  f897feeb  bl 0x10869f7c
108c3f98  0050a0e1  cpy r5, r0
108c3f9c  0720a0e1  cpy r2, r7
108c3fa0  0210a0e3  mov r1, #0x2
108c3fa4  040080e2  add r0, r0, #0x4
108c3fa8  c924feeb  bl 0x1084d2d4
108c3fac  0410a0e1  cpy r1, r4
108c3fb0  040085e2  add r0, r5, #0x4
108c3fb4  b824feeb  bl 0x1084d29c
108c3fb8  0510a0e1  cpy r1, r5
108c3fbc  0600a0e1  cpy r0, r6
108c3fc0  e08efeeb  bl 0x10867b48
108c3fc4  0420a0e1  cpy r2, r4
108c3fc8  0600a0e1  cpy r0, r6
108c3fcc  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108c3fd0  0210a0e3  mov r1, #0x2
108c3fd4  5b22feea  b 0x1084c948
108c3fd8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
