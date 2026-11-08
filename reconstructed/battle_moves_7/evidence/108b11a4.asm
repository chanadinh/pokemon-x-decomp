108b11a4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b11a8  0150a0e1  cpy r5, r1
108b11ac  0240a0e1  cpy r4, r2
108b11b0  0300a0e3  mov r0, #0x3
108b11b4  d8e9feeb  bl 0x1086b91c
108b11b8  040050e1  cmp r0, r4
108b11bc  0600001a  bne 0x108b11dc
108b11c0  0600a0e3  mov r0, #0x6
108b11c4  d4e9feeb  bl 0x1086b91c
108b11c8  ff2000e2  and r2, r0, #0xff
108b11cc  0410a0e1  cpy r1, r4
108b11d0  0500a0e1  cpy r0, r5
108b11d4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b11d8  061500ea  b 0x108b65f8
108b11dc  7080bde8  ldmia sp!,{r4,r5,r6,pc}
