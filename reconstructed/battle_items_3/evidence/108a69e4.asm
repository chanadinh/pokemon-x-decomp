108a69e4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a69e8  0150a0e1  cpy r5, r1
108a69ec  0240a0e1  cpy r4, r2
108a69f0  0300a0e3  mov r0, #0x3
108a69f4  c813ffeb  bl 0x1086b91c
108a69f8  040050e1  cmp r0, r4
108a69fc  1100001a  bne 0x108a6a48
108a6a00  0410a0e1  cpy r1, r4
108a6a04  0500a0e1  cpy r0, r5
108a6a08  a4b6feeb  bl 0x108544a0
108a6a0c  bc00d0e1  ldrh r0, [r0,#0xc]
108a6a10  011c40e2  sub r1, r0, #0x100
108a6a14  e71051e2  subs r1, r1, #0xe7
108a6a18  0a00001a  bne 0x108a6a48
108a6a1c  1600a0e3  mov r0, #0x16
108a6a20  bd13ffeb  bl 0x1086b91c
108a6a24  ff0000e2  and r0, r0, #0xff
108a6a28  0f0050e3  cmp r0, #0xf
108a6a2c  0100000a  beq 0x108a6a38
108a6a30  070050e3  cmp r0, #0x7
108a6a34  0300001a  bne 0x108a6a48
108a6a38  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a6a3c  3200a0e3  mov r0, #0x32
108a6a40  04109fe5  ldr r1, [0x108a6a4c]
108a6a44  0e99feea  b 0x1084ce84
108a6a48  7080bde8  ldmia sp!,{r4,r5,r6,pc}
