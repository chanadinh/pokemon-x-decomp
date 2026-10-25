00127b6c  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
00127b70  084080e2  add r4, r0, #0x8
00127b74  0150a0e1  cpy r5, r1
00127b78  080090e5  ldr r0, [r0,#0x8]
00127b7c  0260a0e1  cpy r6, r2
00127b80  281090e5  ldr r1, [r0,#0x28]
00127b84  0400a0e1  cpy r0, r4
00127b88  31ff2fe1  blx r1
00127b8c  000050e3  cmp r0, #0x0
00127b90  0000a0e3  mov r0, #0x0
00127b94  0e00000a  beq 0x00127bd4
00127b98  0070a0e1  cpy r7, r0
00127b9c  00008de5  str r0, [sp,#0x0]
00127ba0  0530a0e1  cpy r3, r5
00127ba4  0620a0e1  cpy r2, r6
00127ba8  0d10a0e1  cpy r1, sp
00127bac  100084e2  add r0, r4, #0x10
00127bb0  9f2e00eb  bl 0x00133634
00127bb4  021100e2  and r1, r0, #0x80000000
00127bb8  000051e3  cmp r1, #0x0
00127bbc  00009da5  ldrge r0, [sp,#0x0]
00127bc0  030000aa  bge 0x00127bd4
00127bc4  0f10a0e1  cpy r1, pc
00127bc8  a02fb0e1  movs r2, r0, lsr #0x1f
00127bcc  4cf2ff1b  blne 0x00124504
00127bd0  0700a0e1  cpy r0, r7
00127bd4  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
