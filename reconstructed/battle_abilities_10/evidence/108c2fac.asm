108c2fac  70402de9  stmdb sp!,{r4,r5,r6,lr}
108c2fb0  0140a0e1  cpy r4, r1
108c2fb4  0100a0e1  cpy r0, r1
108c2fb8  0250a0e1  cpy r5, r2
108c2fbc  0210a0e1  cpy r1, r2
108c2fc0  3645feeb  bl 0x108544a0
108c2fc4  bc00d0e1  ldrh r0, [r0,#0xc]
108c2fc8  011c40e2  sub r1, r0, #0x100
108c2fcc  a51051e2  subs r1, r1, #0xa5
108c2fd0  1200001a  bne 0x108c3020
108c2fd4  0400a0e1  cpy r0, r4
108c2fd8  9e23feeb  bl 0x1084be58
108c2fdc  010050e3  cmp r0, #0x1
108c2fe0  0e00001a  bne 0x108c3020
108c2fe4  0400a0e3  mov r0, #0x4
108c2fe8  4ba2feeb  bl 0x1086b91c
108c2fec  ff1000e2  and r1, r0, #0xff
108c2ff0  0500a0e1  cpy r0, r5
108c2ff4  6b25feeb  bl 0x1084c5a8
108c2ff8  000050e3  cmp r0, #0x0
108c2ffc  0700000a  beq 0x108c3020
108c3000  1b00a0e3  mov r0, #0x1b
108c3004  44a2feeb  bl 0x1086b91c
108c3008  020050e3  cmp r0, #0x2
108c300c  0300001a  bne 0x108c3020
108c3010  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108c3014  061ba0e3  mov r1, #0x1800
108c3018  3600a0e3  mov r0, #0x36
108c301c  9827feea  b 0x1084ce84
108c3020  7080bde8  ldmia sp!,{r4,r5,r6,pc}
