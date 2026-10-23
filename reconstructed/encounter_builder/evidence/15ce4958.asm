15ce4958  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
15ce495c  0050a0e1  cpy r5,r0
15ce4960  0170a0e1  cpy r7,r1
15ce4964  6c0090e5  ldr r0,[r0,#0x6c]
15ce4968  0260a0e1  cpy r6,r2
15ce496c  000050e3  cmp r0,#0x0
15ce4970  0040a013  movne r4,#0x0
15ce4974  0c00000a  beq 0x15ce49ac
15ce4978  090000da  ble 0x15ce49a4
15ce497c  840184e0  add r0,r4,r4, lsl #0x3
15ce4980  0630a0e1  cpy r3,r6
15ce4984  802085e0  add r2,r5,r0, lsl #0x1
15ce4988  0710a0e1  cpy r1,r7
15ce498c  0500a0e1  cpy r0,r5
15ce4990  1e0000eb  bl 0x15ce4a10
15ce4994  6c0095e5  ldr r0,[r5,#0x6c]
15ce4998  014084e2  add r4,r4,#0x1
15ce499c  040050e1  cmp r0,r4
15ce49a0  f5ffffca  bgt 0x15ce497c
15ce49a4  0100a0e3  mov r0,#0x1
15ce49a8  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
15ce49ac  0020a0e3  mov r2,#0x0
15ce49b0  0210a0e1  cpy r1,r2
15ce49b4  0200a0e1  cpy r0,r2
15ce49b8  106efceb  bl 0x15c00200
15ce49bc  0000a0e3  mov r0,#0x0
15ce49c0  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
