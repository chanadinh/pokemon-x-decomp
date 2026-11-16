108aa3d4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aa3d8  0160a0e1  cpy r6, r1
108aa3dc  0240a0e1  cpy r4, r2
108aa3e0  0200a0e3  mov r0, #0x2
108aa3e4  4c05ffeb  bl 0x1086b91c
108aa3e8  040050e1  cmp r0, r4
108aa3ec  1c00001a  bne 0x108aa464
108aa3f0  0410a0e1  cpy r1, r4
108aa3f4  0600a0e1  cpy r0, r6
108aa3f8  9e89feeb  bl 0x1084ca78
108aa3fc  000050e3  cmp r0, #0x0
108aa400  1700000a  beq 0x108aa464
108aa404  0420a0e1  cpy r2, r4
108aa408  0310a0e3  mov r1, #0x3
108aa40c  0600a0e1  cpy r0, r6
108aa410  d9fefeeb  bl 0x10869f7c
108aa414  0050a0e1  cpy r5, r0
108aa418  48209fe5  ldr r2, [0x108aa468]
108aa41c  0210a0e3  mov r1, #0x2
108aa420  040080e2  add r0, r0, #0x4
108aa424  aa8bfeeb  bl 0x1084d2d4
108aa428  0410a0e1  cpy r1, r4
108aa42c  040085e2  add r0, r5, #0x4
108aa430  998bfeeb  bl 0x1084d29c
108aa434  0510a0e1  cpy r1, r5
108aa438  0600a0e1  cpy r0, r6
108aa43c  c1f5feeb  bl 0x10867b48
108aa440  0420a0e1  cpy r2, r4
108aa444  2a10a0e3  mov r1, #0x2a
108aa448  0600a0e1  cpy r0, r6
108aa44c  cafefeeb  bl 0x10869f7c
108aa450  0010a0e1  cpy r1, r0
108aa454  4c40c0e5  strb r4, [r0,#0x4c]
108aa458  0600a0e1  cpy r0, r6
108aa45c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aa460  b8f5feea  b 0x10867b48
108aa464  7080bde8  ldmia sp!,{r4,r5,r6,pc}
