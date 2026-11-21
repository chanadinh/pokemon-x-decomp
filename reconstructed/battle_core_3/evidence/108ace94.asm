108ace94  3c209fe5  ldr r2, [0x108aced8]
108ace98  000280e0  add r0, r0, r0, lsl #0x4
108ace9c  10402de9  stmdb sp!,{r4,lr}
108acea0  000282e0  add r0, r2, r0, lsl #0x4
108acea4  014280e0  add r4, r0, r1, lsl #0x4
108acea8  000094e5  ldr r0, [r4,#0x0]
108aceac  000050e3  cmp r0, #0x0
108aceb0  0700000a  beq 0x108aced4
108aceb4  c480feeb  bl 0x1084d1cc
108aceb8  041094e5  ldr r1, [r4,#0x4]
108acebc  0000a0e3  mov r0, #0x0
108acec0  000084e5  str r0, [r4,#0x0]
108acec4  0710c1e3  bic r1, r1, #0x7
108acec8  0410a4e5  str r1, [r4,#0x4]!
108acecc  080084e5  str r0, [r4,#0x8]
108aced0  0100a0e3  mov r0, #0x1
108aced4  1080bde8  ldmia sp!,{r4,pc}
