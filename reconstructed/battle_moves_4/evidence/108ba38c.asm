108ba38c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba390  0050a0e1  cpy r5, r0
108ba394  0240a0e1  cpy r4, r2
108ba398  0200a0e3  mov r0, #0x2
108ba39c  5ec5feeb  bl 0x1086b91c
108ba3a0  040050e1  cmp r0, r4
108ba3a4  0200001a  bne 0x108ba3b4
108ba3a8  0500a0e1  cpy r0, r5
108ba3ac  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba3b0  854bfeea  b 0x1084d1cc
108ba3b4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
