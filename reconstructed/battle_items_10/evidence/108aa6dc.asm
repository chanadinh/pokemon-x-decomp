108aa6dc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aa6e0  0060a0e1  cpy r6, r0
108aa6e4  0150a0e1  cpy r5, r1
108aa6e8  0240a0e1  cpy r4, r2
108aa6ec  2f00a0e3  mov r0, #0x2f
108aa6f0  8904ffeb  bl 0x1086b91c
108aa6f4  080010e3  tst r0, #0x8
108aa6f8  1000000a  beq 0x108aa740
108aa6fc  0200a0e3  mov r0, #0x2
108aa700  8504ffeb  bl 0x1086b91c
108aa704  040050e1  cmp r0, r4
108aa708  0c00001a  bne 0x108aa740
108aa70c  0410a0e1  cpy r1, r4
108aa710  0500a0e1  cpy r0, r5
108aa714  61a7feeb  bl 0x108544a0
108aa718  8886feeb  bl 0x1084c140
108aa71c  000050e3  cmp r0, #0x0
108aa720  0600000a  beq 0x108aa740
108aa724  0600a0e1  cpy r0, r6
108aa728  8f88feeb  bl 0x1084c96c
108aa72c  0420a0e1  cpy r2, r4
108aa730  0500a0e1  cpy r0, r5
108aa734  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aa738  0010a0e3  mov r1, #0x0
108aa73c  8188feea  b 0x1084c948
108aa740  7080bde8  ldmia sp!,{r4,r5,r6,pc}
