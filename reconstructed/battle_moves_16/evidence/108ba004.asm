108ba004  70402de9  stmdb sp!,{r4,r5,r6,lr}
108ba008  0240a0e1  cpy r4, r2
108ba00c  0350a0e1  cpy r5, r3
108ba010  0200a0e3  mov r0, #0x2
108ba014  40c6feeb  bl 0x1086b91c
108ba018  040050e1  cmp r0, r4
108ba01c  0600001a  bne 0x108ba03c
108ba020  000095e5  ldr r0, [r5,#0x0]
108ba024  000050e3  cmp r0, #0x0
108ba028  0300000a  beq 0x108ba03c
108ba02c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108ba030  0110a0e3  mov r1, #0x1
108ba034  5300a0e3  mov r0, #0x53
108ba038  25aefeea  b 0x108658d4
108ba03c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
