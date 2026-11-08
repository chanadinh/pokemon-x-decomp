108b59f0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108b59f4  0060a0e1  cpy r6, r0
108b59f8  10d04de2  sub sp, sp, #0x10
108b59fc  0170a0e1  cpy r7, r1
108b5a00  0250a0e1  cpy r5, r2
108b5a04  0380a0e1  cpy r8, r3
108b5a08  0500a0e3  mov r0, #0x5
108b5a0c  0a59feeb  bl 0x1084be3c
108b5a10  0040a0e1  cpy r4, r0
108b5a14  0500a0e1  cpy r0, r5
108b5a18  6859feeb  bl 0x1084bfc0
108b5a1c  8610a0e3  mov r1, #0x86
108b5a20  0c108de5  str r1, [sp,#0xc]
108b5a24  0110a0e3  mov r1, #0x1
108b5a28  13008de8  stmia sp,{r0,r1,r4}
108b5a2c  0830a0e1  cpy r3, r8
108b5a30  0520a0e1  cpy r2, r5
108b5a34  0710a0e1  cpy r1, r7
108b5a38  0600a0e1  cpy r0, r6
108b5a3c  9b58feeb  bl 0x1084bcb0
108b5a40  10d08de2  add sp, sp, #0x10
108b5a44  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
