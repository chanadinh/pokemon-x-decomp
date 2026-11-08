108bbd78  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bbd7c  0160a0e1  cpy r6, r1
108bbd80  0250a0e1  cpy r5, r2
108bbd84  0300a0e3  mov r0, #0x3
108bbd88  e3befeeb  bl 0x1086b91c
108bbd8c  050050e1  cmp r0, r5
108bbd90  1600001a  bne 0x108bbdf0
108bbd94  0600a0e3  mov r0, #0x6
108bbd98  dfbefeeb  bl 0x1086b91c
108bbd9c  ff4000e2  and r4, r0, #0xff
108bbda0  0410a0e1  cpy r1, r4
108bbda4  0600a0e1  cpy r0, r6
108bbda8  bc61feeb  bl 0x108544a0
108bbdac  0210a0e3  mov r1, #0x2
108bbdb0  346bfeeb  bl 0x10856a88
108bbdb4  000050e3  cmp r0, #0x0
108bbdb8  0c00000a  beq 0x108bbdf0
108bbdbc  0520a0e1  cpy r2, r5
108bbdc0  0a10a0e3  mov r1, #0xa
108bbdc4  0600a0e1  cpy r0, r6
108bbdc8  6bb8feeb  bl 0x10869f7c
108bbdcc  0010a0e1  cpy r1, r0
108bbdd0  0120a0e3  mov r2, #0x1
108bbdd4  1120c1e5  strb r2, [r1,#0x11]
108bbdd8  0200a0e3  mov r0, #0x2
108bbddc  0540c1e5  strb r4, [r1,#0x5]
108bbde0  0400c1e5  strb r0, [r1,#0x4]
108bbde4  0600a0e1  cpy r0, r6
108bbde8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bbdec  55affeea  b 0x10867b48
108bbdf0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
