108c4d34  f8432de9  stmdb sp!,{r3,r4,r5,r6,r7,r8,r9,lr}
108c4d38  0150a0e1  cpy r5, r1
108c4d3c  0060a0e1  cpy r6, r0
108c4d40  0240a0e1  cpy r4, r2
108c4d44  000093e5  ldr r0, [r3,#0x0]
108c4d48  000050e3  cmp r0, #0x0
108c4d4c  1800000a  beq 0x108c4db4
108c4d50  0100a0e1  cpy r0, r1
108c4d54  0210a0e1  cpy r1, r2
108c4d58  d03dfeeb  bl 0x108544a0
108c4d5c  bc00d0e1  ldrh r0, [r0,#0xc]
108c4d60  011c40e2  sub r1, r0, #0x100
108c4d64  a51051e2  subs r1, r1, #0xa5
108c4d68  0070a0e3  mov r7, #0x0
108c4d6c  1000001a  bne 0x108c4db4
108c4d70  0200a0e3  mov r0, #0x2
108c4d74  e89afeeb  bl 0x1086b91c
108c4d78  040050e1  cmp r0, r4
108c4d7c  0c00001a  bne 0x108c4db4
108c4d80  1000a0e3  mov r0, #0x10
108c4d84  e49afeeb  bl 0x1086b91c
108c4d88  0080a0e1  cpy r8, r0
108c4d8c  0600a0e1  cpy r0, r6
108c4d90  b803d0e1  ldrh r0, [r0,#0x38]
108c4d94  000058e1  cmp r8, r0
108c4d98  0500000a  beq 0x108c4db4
108c4d9c  0030a0e3  mov r3, #0x0
108c4da0  0420a0e1  cpy r2, r4
108c4da4  0510a0e1  cpy r1, r5
108c4da8  0600a0e1  cpy r0, r6
108c4dac  00708de5  str r7, [sp,#0x0]
108c4db0  e316feeb  bl 0x1084a944
108c4db4  f883bde8  ldmia sp!,{r3,r4,r5,r6,r7,r8,r9,pc}
