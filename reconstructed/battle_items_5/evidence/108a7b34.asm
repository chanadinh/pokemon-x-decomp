108a7b34  70402de9  stmdb sp!,{r4,r5,r6,lr}
108a7b38  0150a0e1  cpy r5, r1
108a7b3c  0240a0e1  cpy r4, r2
108a7b40  0300a0e3  mov r0, #0x3
108a7b44  740fffeb  bl 0x1086b91c
108a7b48  040050e1  cmp r0, r4
108a7b4c  1000001a  bne 0x108a7b94
108a7b50  0410a0e1  cpy r1, r4
108a7b54  0500a0e1  cpy r0, r5
108a7b58  50b2feeb  bl 0x108544a0
108a7b5c  bc00d0e1  ldrh r0, [r0,#0xc]
108a7b60  011c40e2  sub r1, r0, #0x100
108a7b64  6e1051e2  subs r1, r1, #0x6e
108a7b68  0900001a  bne 0x108a7b94
108a7b6c  1200a0e3  mov r0, #0x12
108a7b70  690fffeb  bl 0x1086b91c
108a7b74  7000ffe6  uxth r0, r0
108a7b78  1763fdeb  bl 0x108007dc
108a7b7c  020050e3  cmp r0, #0x2
108a7b80  0300001a  bne 0x108a7b94
108a7b84  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108a7b88  021aa0e3  mov r1, #0x2000
108a7b8c  3600a0e3  mov r0, #0x36
108a7b90  bb94feea  b 0x1084ce84
108a7b94  7080bde8  ldmia sp!,{r4,r5,r6,pc}
