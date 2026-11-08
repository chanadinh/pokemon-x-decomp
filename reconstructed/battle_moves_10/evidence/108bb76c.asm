108bb76c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb770  0150a0e1  cpy r5, r1
108bb774  0240a0e1  cpy r4, r2
108bb778  0200a0e3  mov r0, #0x2
108bb77c  66c0feeb  bl 0x1086b91c
108bb780  040050e1  cmp r0, r4
108bb784  1200001a  bne 0x108bb7d4
108bb788  0410a0e1  cpy r1, r4
108bb78c  0500a0e1  cpy r0, r5
108bb790  883ffeeb  bl 0x1084b5b8
108bb794  000050e3  cmp r0, #0x0
108bb798  0900000a  beq 0x108bb7c4
108bb79c  0410a0e1  cpy r1, r4
108bb7a0  0500a0e1  cpy r0, r5
108bb7a4  3d63feeb  bl 0x108544a0
108bb7a8  b201d0e1  ldrh r0, [r0,#0x12]
108bb7ac  0040a0e1  cpy r4, r0
108bb7b0  0710a0e3  mov r1, #0x7
108bb7b4  ee45feeb  bl 0x1084cf74
108bb7b8  000054e3  cmp r4, #0x0
108bb7bc  00005013  cmpne r0, #0x0
108bb7c0  0300001a  bne 0x108bb7d4
108bb7c4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb7c8  1b10a0e3  mov r1, #0x1b
108bb7cc  2300a0e3  mov r0, #0x23
108bb7d0  3fa8feea  b 0x108658d4
108bb7d4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
