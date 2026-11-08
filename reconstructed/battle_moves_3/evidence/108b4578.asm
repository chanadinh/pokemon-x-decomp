108b4578  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b457c  0150a0e1  cpy r5, r1
108b4580  0240a0e1  cpy r4, r2
108b4584  0300a0e3  mov r0, #0x3
108b4588  e3dcfeeb  bl 0x1086b91c
108b458c  040050e1  cmp r0, r4
108b4590  0800001a  bne 0x108b45b8
108b4594  0410a0e1  cpy r1, r4
108b4598  0500a0e1  cpy r0, r5
108b459c  bf7ffeeb  bl 0x108544a0
108b45a0  0f10a0e3  mov r1, #0xf
108b45a4  9dfffeeb  bl 0x10874420
108b45a8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b45ac  ff1000e2  and r1, r0, #0xff
108b45b0  3800a0e3  mov r0, #0x38
108b45b4  c6c4feea  b 0x108658d4
108b45b8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
