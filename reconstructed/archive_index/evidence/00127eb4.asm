00127eb4  f0402de9  stmdb sp!,{r4,r5,r6,r7,lr}
00127eb8  14d04de2  sub sp, sp, #0x14
00127ebc  0240a0e1  cpy r4, r2
00127ec0  28509de5  ldr r5, [sp,#0x28]
00127ec4  0070a0e1  cpy r7, r0
00127ec8  0360a0e1  cpy r6, r3
00127ecc  04208de2  add r2, sp, #0x4
00127ed0  7dffffeb  bl 0x00127ccc
00127ed4  0000a0e3  mov r0, #0x0
00127ed8  00008de5  str r0, [sp,#0x0]
00127edc  0c009de5  ldr r0, [sp,#0xc]
00127ee0  0530a0e1  cpy r3, r5
00127ee4  0620a0e1  cpy r2, r6
00127ee8  041080e0  add r1, r0, r4
00127eec  0700a0e1  cpy r0, r7
00127ef0  59ffffeb  bl 0x00127c5c
00127ef4  14d08de2  add sp, sp, #0x14
00127ef8  f080bde8  ldmia sp!,{r4,r5,r6,r7,pc}
