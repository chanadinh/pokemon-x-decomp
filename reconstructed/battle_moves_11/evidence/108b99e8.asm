108b99e8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b99ec  0160a0e1  cpy r6, r1
108b99f0  0240a0e1  cpy r4, r2
108b99f4  0200a0e3  mov r0, #0x2
108b99f8  c7c7feeb  bl 0x1086b91c
108b99fc  040050e1  cmp r0, r4
108b9a00  1200001a  bne 0x108b9a50
108b9a04  0600a0e1  cpy r0, r6
108b9a08  3e8dfdeb  bl 0x1081cf08
108b9a0c  0050b0e1  movs r5, r0
108b9a10  0e00000a  beq 0x108b9a50
108b9a14  42abffeb  bl 0x108a4724
108b9a18  000050e3  cmp r0, #0x0
108b9a1c  0b00001a  bne 0x108b9a50
108b9a20  0520a0e1  cpy r2, r5
108b9a24  0410a0e1  cpy r1, r4
108b9a28  0600a0e1  cpy r0, r6
108b9a2c  5ee0feeb  bl 0x10871bac
108b9a30  0040a0e1  cpy r4, r0
108b9a34  0510a0e1  cpy r1, r5
108b9a38  1200a0e3  mov r0, #0x12
108b9a3c  a4affeeb  bl 0x108658d4
108b9a40  0410a0e1  cpy r1, r4
108b9a44  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b9a48  0d00a0e3  mov r0, #0xd
108b9a4c  a0affeea  b 0x108658d4
108b9a50  7080bde8  ldmia sp!,{r4,r5,r6,pc}
