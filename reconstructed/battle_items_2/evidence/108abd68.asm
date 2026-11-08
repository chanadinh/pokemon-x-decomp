108abd68  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108abd6c  0140a0e1  cpy r4, r1
108abd70  0060a0e1  cpy r6, r0
108abd74  000093e5  ldr r0, [r3,#0x0]
108abd78  0250a0e1  cpy r5, r2
108abd7c  000050e3  cmp r0, #0x0
108abd80  0d00001a  bne 0x108abdbc
108abd84  0210a0e1  cpy r1, r2
108abd88  0400a0e1  cpy r0, r4
108abd8c  c3a1feeb  bl 0x108544a0
108abd90  0f10a0e3  mov r1, #0xf
108abd94  3babfeeb  bl 0x10856a88
108abd98  000050e3  cmp r0, #0x0
108abd9c  0600001a  bne 0x108abdbc
108abda0  0100a0e3  mov r0, #0x1
108abda4  00008de5  str r0, [sp,#0x0]
108abda8  0230a0e3  mov r3, #0x2
108abdac  0520a0e1  cpy r2, r5
108abdb0  0410a0e1  cpy r1, r4
108abdb4  0600a0e1  cpy r0, r6
108abdb8  f680feeb  bl 0x1084c198
108abdbc  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
