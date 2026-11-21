108b5be0  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b5be4  0160a0e1  cpy r6, r1
108b5be8  0240a0e1  cpy r4, r2
108b5bec  0300a0e3  mov r0, #0x3
108b5bf0  49d7feeb  bl 0x1086b91c
108b5bf4  040050e1  cmp r0, r4
108b5bf8  1b00001a  bne 0x108b5c6c
108b5bfc  0410a0e1  cpy r1, r4
108b5c00  0600a0e1  cpy r0, r6
108b5c04  9b5bfeeb  bl 0x1084ca78
108b5c08  000050e3  cmp r0, #0x0
108b5c0c  1600000a  beq 0x108b5c6c
108b5c10  0420a0e1  cpy r2, r4
108b5c14  1f10a0e3  mov r1, #0x1f
108b5c18  0600a0e1  cpy r0, r6
108b5c1c  d6d0feeb  bl 0x10869f7c
108b5c20  0050a0e1  cpy r5, r0
108b5c24  0200a0e3  mov r0, #0x2
108b5c28  0400c5e5  strb r0, [r5,#0x4]
108b5c2c  0410a0e1  cpy r1, r4
108b5c30  0600a0e1  cpy r0, r6
108b5c34  47f5feeb  bl 0x10873158
108b5c38  0500c5e5  strb r0, [r5,#0x5]
108b5c3c  0510a0e1  cpy r1, r5
108b5c40  0600a0e1  cpy r0, r6
108b5c44  bfc7feeb  bl 0x10867b48
108b5c48  0420a0e1  cpy r2, r4
108b5c4c  1210a0e3  mov r1, #0x12
108b5c50  0600a0e1  cpy r0, r6
108b5c54  c8d0feeb  bl 0x10869f7c
108b5c58  0010a0e1  cpy r1, r0
108b5c5c  0440c0e5  strb r4, [r0,#0x4]
108b5c60  0600a0e1  cpy r0, r6
108b5c64  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b5c68  b6c7feea  b 0x10867b48
108b5c6c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
