108a6b14  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a6b18  0060a0e1  cpy r6, r0
108a6b1c  0150a0e1  cpy r5, r1
108a6b20  0240a0e1  cpy r4, r2
108a6b24  0300a0e3  mov r0, #0x3
108a6b28  7b13ffeb  bl 0x1086b91c
108a6b2c  040050e1  cmp r0, r4
108a6b30  1300001a  bne 0x108a6b84
108a6b34  0410a0e1  cpy r1, r4
108a6b38  0500a0e1  cpy r0, r5
108a6b3c  57b6feeb  bl 0x108544a0
108a6b40  bc00d0e1  ldrh r0, [r0,#0xc]
108a6b44  011c40e2  sub r1, r0, #0x100
108a6b48  e31051e2  subs r1, r1, #0xe3
108a6b4c  0c00001a  bne 0x108a6b84
108a6b50  1600a0e3  mov r0, #0x16
108a6b54  7013ffeb  bl 0x1086b91c
108a6b58  ff0000e2  and r0, r0, #0xff
108a6b5c  0f0050e3  cmp r0, #0xf
108a6b60  0100000a  beq 0x108a6b6c
108a6b64  080050e3  cmp r0, #0x8
108a6b68  0500001a  bne 0x108a6b84
108a6b6c  0600a0e1  cpy r0, r6
108a6b70  1b98feeb  bl 0x1084cbe4
108a6b74  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6b78  0010a0e1  cpy r1, r0
108a6b7c  3200a0e3  mov r0, #0x32
108a6b80  bf98feea  b 0x1084ce84
108a6b84  7080bde8  ldmia sp!,{r4,r5,r6,pc}
