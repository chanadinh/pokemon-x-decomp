108aba18  70402de9  stmdb sp!,{r4,r5,r6,lr}
108aba1c  0060a0e1  cpy r6, r0
108aba20  0150a0e1  cpy r5, r1
108aba24  0240a0e1  cpy r4, r2
108aba28  0200a0e3  mov r0, #0x2
108aba2c  bafffeeb  bl 0x1086b91c
108aba30  040050e1  cmp r0, r4
108aba34  1100001a  bne 0x108aba80
108aba38  0420a0e3  mov r2, #0x4
108aba3c  0410a0e1  cpy r1, r4
108aba40  0500a0e1  cpy r0, r5
108aba44  3983feeb  bl 0x1084c730
108aba48  000050e3  cmp r0, #0x0
108aba4c  0b00000a  beq 0x108aba80
108aba50  0210a0e3  mov r1, #0x2
108aba54  1100a0e3  mov r0, #0x11
108aba58  9de7feeb  bl 0x108658d4
108aba5c  000050e3  cmp r0, #0x0
108aba60  0600000a  beq 0x108aba80
108aba64  0600a0e1  cpy r0, r6
108aba68  bf83feeb  bl 0x1084c96c
108aba6c  0420a0e1  cpy r2, r4
108aba70  0500a0e1  cpy r0, r5
108aba74  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108aba78  0010a0e3  mov r1, #0x0
108aba7c  b183feea  b 0x1084c948
108aba80  7080bde8  ldmia sp!,{r4,r5,r6,pc}
