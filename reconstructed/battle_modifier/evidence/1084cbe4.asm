1084cbe4  10402de9  stmdb sp!,{r4,lr}
1084cbe8  0340a0e3  mov r4, #0x3
1084cbec  b803d0e1  ldrh r0, [r0,#0x38]
1084cbf0  0410a0e1  cpy r1, r4
1084cbf4  de0000eb  bl 0x1084cf74
1084cbf8  641080e2  add r1, r0, #0x64
1084cbfc  0006a0e1  mov r0, r0, lsl #0xc
1084cc00  190980e2  add r0, r0, #0x64000
1084cc04  900a00ee  vmov s1, r0
1084cc08  0a0a9fed  vldr.32 s0, [pc,#0x28]
1084cc0c  000051e3  cmp r1, #0x0
1084cc10  24109fe5  ldr r1, [0x1084cc3c]
1084cc14  e00af8ee  vcvt.f32.s32 s1, s1
1084cc18  800a30ce  vaddgt.f32 s0, s1, s0
1084cc1c  c00a30de  vsuble.f32 s0, s1, s0
1084cc20  c00abdee  vcvt.s32.f32 s0, s0
1084cc24  100a10ee  vmov r0, s0
1084cc28  9110c0e0  smull r1, r0, r1, r0
1084cc2c  c012a0e1  mov r1, r0, asr #0x5
1084cc30  c00f41e0  sub r0, r1, r0, asr #0x1f
1084cc34  1080bde8  ldmia sp!,{r4,pc}
