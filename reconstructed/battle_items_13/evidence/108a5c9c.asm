108a5c9c  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108a5ca0  0260a0e1  cpy r6, r2
108a5ca4  0080a0e1  cpy r8, r0
108a5ca8  0170a0e1  cpy r7, r1
108a5cac  0400a0e3  mov r0, #0x4
108a5cb0  1917ffeb  bl 0x1086b91c
108a5cb4  060050e1  cmp r0, r6
108a5cb8  2600001a  bne 0x108a5d58
108a5cbc  1e00a0e3  mov r0, #0x1e
108a5cc0  1517ffeb  bl 0x1086b91c
108a5cc4  070050e3  cmp r0, #0x7
108a5cc8  2200001a  bne 0x108a5d58
108a5ccc  0300a0e3  mov r0, #0x3
108a5cd0  1117ffeb  bl 0x1086b91c
108a5cd4  ff5000e2  and r5, r0, #0xff
108a5cd8  1f0055e3  cmp r5, #0x1f
108a5cdc  1d00000a  beq 0x108a5d58
108a5ce0  0620a0e1  cpy r2, r6
108a5ce4  0b10a0e3  mov r1, #0xb
108a5ce8  0700a0e1  cpy r0, r7
108a5cec  a210ffeb  bl 0x10869f7c
108a5cf0  0040a0e1  cpy r4, r0
108a5cf4  0700a0e3  mov r0, #0x7
108a5cf8  0f50c4e5  strb r5, [r4,#0xf]
108a5cfc  b400c4e1  strh r0, [r4,#0x4]
108a5d00  0600a0e1  cpy r0, r6
108a5d04  4544ffeb  bl 0x10876e20
108a5d08  080084e5  str r0, [r4,#0x8]
108a5d0c  0110a0e3  mov r1, #0x1
108a5d10  0c10c4e5  strb r1, [r4,#0xc]
108a5d14  0d10c4e5  strb r1, [r4,#0xd]
108a5d18  3c209fe5  ldr r2, [0x108a5d5c]
108a5d1c  0210a0e3  mov r1, #0x2
108a5d20  140084e2  add r0, r4, #0x14
108a5d24  6a9dfeeb  bl 0x1084d2d4
108a5d28  0510a0e1  cpy r1, r5
108a5d2c  140084e2  add r0, r4, #0x14
108a5d30  599dfeeb  bl 0x1084d29c
108a5d34  0800a0e1  cpy r0, r8
108a5d38  b803d0e1  ldrh r0, [r0,#0x38]
108a5d3c  0010a0e1  cpy r1, r0
108a5d40  140084e2  add r0, r4, #0x14
108a5d44  549dfeeb  bl 0x1084d29c
108a5d48  0410a0e1  cpy r1, r4
108a5d4c  0700a0e1  cpy r0, r7
108a5d50  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108a5d54  7b07ffea  b 0x10867b48
108a5d58  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
