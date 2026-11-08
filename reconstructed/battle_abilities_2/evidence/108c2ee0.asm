108c2ee0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2ee4  0150a0e1  cpy r5, r1
108c2ee8  0240a0e1  cpy r4, r2
108c2eec  0200a0e3  mov r0, #0x2
108c2ef0  89a2feeb  bl 0x1086b91c
108c2ef4  040050e1  cmp r0, r4
108c2ef8  1200001a  bne 0x108c2f48
108c2efc  0420a0e1  cpy r2, r4
108c2f00  1e10a0e3  mov r1, #0x1e
108c2f04  0500a0e1  cpy r0, r5
108c2f08  1b9cfeeb  bl 0x10869f7c
108c2f0c  0040a0e1  cpy r4, r0
108c2f10  000090e5  ldr r0, [r0,#0x0]
108c2f14  0020a0e3  mov r2, #0x0
108c2f18  0420c4e5  strb r2, [r4,#0x4]
108c2f1c  0110a0e3  mov r1, #0x1
108c2f20  020580e3  orr r0, r0, #0x800000
108c2f24  0610c4e5  strb r1, [r4,#0x6]
108c2f28  000084e5  str r0, [r4,#0x0]
108c2f2c  6120a0e3  mov r2, #0x61
108c2f30  080084e2  add r0, r4, #0x8
108c2f34  e628feeb  bl 0x1084d2d4
108c2f38  0410a0e1  cpy r1, r4
108c2f3c  0500a0e1  cpy r0, r5
108c2f40  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c2f44  ff92feea  b 0x10867b48
108c2f48  7080bde8  ldmia sp!,{r4,r5,r6,pc}
