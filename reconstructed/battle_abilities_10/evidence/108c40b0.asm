108c40b0  f8402de9  stmdb sp!,{r3,r4,r5,r6,r7,lr}
108c40b4  0140a0e1  cpy r4, r1
108c40b8  0060a0e1  cpy r6, r0
108c40bc  000093e5  ldr r0, [r3,#0x0]
108c40c0  0250a0e1  cpy r5, r2
108c40c4  000050e3  cmp r0, #0x0
108c40c8  0d00000a  beq 0x108c4104
108c40cc  0100a0e1  cpy r0, r1
108c40d0  0210a0e1  cpy r1, r2
108c40d4  f140feeb  bl 0x108544a0
108c40d8  bc00d0e1  ldrh r0, [r0,#0xc]
108c40dc  012c40e2  sub r2, r0, #0x100
108c40e0  a52052e2  subs r2, r2, #0xa5
108c40e4  0010a0e3  mov r1, #0x0
108c40e8  0500001a  bne 0x108c4104
108c40ec  00108de5  str r1, [sp,#0x0]
108c40f0  0030a0e3  mov r3, #0x0
108c40f4  0520a0e1  cpy r2, r5
108c40f8  0410a0e1  cpy r1, r4
108c40fc  0600a0e1  cpy r0, r6
108c4100  0f1afeeb  bl 0x1084a944
108c4104  f880bde8  ldmia sp!,{r3,r4,r5,r6,r7,pc}
