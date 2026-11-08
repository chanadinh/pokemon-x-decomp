108a7598  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a759c  0040a0e1  cpy r4, r0
108a75a0  0250a0e1  cpy r5, r2
108a75a4  0400a0e3  mov r0, #0x4
108a75a8  db10ffeb  bl 0x1086b91c
108a75ac  050050e1  cmp r0, r5
108a75b0  1500001a  bne 0x108a760c
108a75b4  0400a0e1  cpy r0, r4
108a75b8  0350a0e3  mov r5, #0x3
108a75bc  b803d0e1  ldrh r0, [r0,#0x38]
108a75c0  0510a0e1  cpy r1, r5
108a75c4  6a96feeb  bl 0x1084cf74
108a75c8  640070e2  rsbs r0, r0, #0x64
108a75cc  0f0adfed  vldr.32 s1, [pc,#0x3c]
108a75d0  0f0a9f0d  vldreq.32 s0, [pc,#0x3c]
108a75d4  0300000a  beq 0x108a75e8
108a75d8  0006a0e1  mov r0, r0, lsl #0xc
108a75dc  100a00ee  vmov s0, r0
108a75e0  400ab8ee  vcvt.f32.u32 s0, s0
108a75e4  200a30ee  vadd.f32 s0, s0, s1
108a75e8  28109fe5  ldr r1, [0x108a7618]
108a75ec  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a75f0  c00abdee  vcvt.s32.f32 s0, s0
108a75f4  100a10ee  vmov r0, s0
108a75f8  9110c0e0  smull r1, r0, r1, r0
108a75fc  c012a0e1  mov r1, r0, asr #0x5
108a7600  c01f41e0  sub r1, r1, r0, asr #0x1f
108a7604  3600a0e3  mov r0, #0x36
108a7608  1d96feea  b 0x1084ce84
108a760c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
