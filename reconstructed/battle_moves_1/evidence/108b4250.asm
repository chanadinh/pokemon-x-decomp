108b4250  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b4254  0150a0e1  cpy r5, r1
108b4258  0240a0e1  cpy r4, r2
108b425c  0300a0e3  mov r0, #0x3
108b4260  adddfeeb  bl 0x1086b91c
108b4264  040050e1  cmp r0, r4
108b4268  0f00001a  bne 0x108b42ac
108b426c  0400a0e3  mov r0, #0x4
108b4270  a9ddfeeb  bl 0x1086b91c
108b4274  ff1000e2  and r1, r0, #0xff
108b4278  0500a0e1  cpy r0, r5
108b427c  8780feeb  bl 0x108544a0
108b4280  46f4feeb  bl 0x108713a0
108b4284  0010a0e1  cpy r1, r0
108b4288  7800a0e3  mov r0, #0x78
108b428c  d9a8ffeb  bl 0x1089e5f8
108b4290  18109fe5  ldr r1, [0x108b42b0]
108b4294  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b4298  911080e0  umull r1, r0, r1, r0
108b429c  a012b0e1  movs r1, r0, lsr #0x5
108b42a0  0110a003  moveq r1, #0x1
108b42a4  3100a0e3  mov r0, #0x31
108b42a8  89c5feea  b 0x108658d4
108b42ac  7080bde8  ldmia sp!,{r4,r5,r6,pc}
