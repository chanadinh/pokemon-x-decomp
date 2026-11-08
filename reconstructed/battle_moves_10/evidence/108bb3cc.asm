108bb3cc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb3d0  0150a0e1  cpy r5, r1
108bb3d4  0240a0e1  cpy r4, r2
108bb3d8  0200a0e3  mov r0, #0x2
108bb3dc  4ec1feeb  bl 0x1086b91c
108bb3e0  040050e1  cmp r0, r4
108bb3e4  1300001a  bne 0x108bb438
108bb3e8  0500a0e1  cpy r0, r5
108bb3ec  0410a0e1  cpy r1, r4
108bb3f0  2a64feeb  bl 0x108544a0
108bb3f4  0010a0e3  mov r1, #0x0
108bb3f8  9345feeb  bl 0x1084ca4c
108bb3fc  000050e3  cmp r0, #0x0
108bb400  0200000a  beq 0x108bb410
108bb404  1b10a0e3  mov r1, #0x1b
108bb408  2300a0e3  mov r0, #0x23
108bb40c  30a9feeb  bl 0x108658d4
108bb410  0500a0e1  cpy r0, r5
108bb414  0410a0e1  cpy r1, r4
108bb418  3041feeb  bl 0x1084b8e0
108bb41c  000050e3  cmp r0, #0x0
108bb420  00f020e3  nop
108bb424  0300000a  beq 0x108bb438
108bb428  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bb42c  1b10a0e3  mov r1, #0x1b
108bb430  2300a0e3  mov r0, #0x23
108bb434  26a9feea  b 0x108658d4
108bb438  7080bde8  ldmia sp!,{r4,r5,r6,pc}
