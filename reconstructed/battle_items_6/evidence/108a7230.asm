108a7230  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7234  0040a0e1  cpy r4, r0
108a7238  0250a0e1  cpy r5, r2
108a723c  0400a0e3  mov r0, #0x4
108a7240  b511ffeb  bl 0x1086b91c
108a7244  050050e1  cmp r0, r5
108a7248  1700001a  bne 0x108a72ac
108a724c  0400a0e1  cpy r0, r4
108a7250  0350a0e3  mov r5, #0x3
108a7254  b803d0e1  ldrh r0, [r0,#0x38]
108a7258  0510a0e1  cpy r1, r5
108a725c  4497feeb  bl 0x1084cf74
108a7260  640060e2  rsb r0, r0, #0x64
108a7264  110adfed  vldr.32 s1, [pc,#0x44]
108a7268  0008a0e1  mov r0, r0, lsl #0x10
108a726c  2008b0e1  movs r0, r0, lsr #0x10
108a7270  0f0a9f0d  vldreq.32 s0, [pc,#0x3c]
108a7274  0300000a  beq 0x108a7288
108a7278  0006a0e1  mov r0, r0, lsl #0xc
108a727c  100a00ee  vmov s0, r0
108a7280  400ab8ee  vcvt.f32.u32 s0, s0
108a7284  200a30ee  vadd.f32 s0, s0, s1
108a7288  28109fe5  ldr r1, [0x108a72b8]
108a728c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7290  c00abdee  vcvt.s32.f32 s0, s0
108a7294  100a10ee  vmov r0, s0
108a7298  9110c0e0  smull r1, r0, r1, r0
108a729c  c012a0e1  mov r1, r0, asr #0x5
108a72a0  c01f41e0  sub r1, r1, r0, asr #0x1f
108a72a4  3600a0e3  mov r0, #0x36
108a72a8  f596feea  b 0x1084ce84
108a72ac  7080bde8  ldmia sp!,{r4,r5,r6,pc}
