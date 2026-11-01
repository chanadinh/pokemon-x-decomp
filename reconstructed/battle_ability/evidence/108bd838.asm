108bd838  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd83c  0150a0e1  cpy r5, r1
108bd840  0240a0e1  cpy r4, r2
108bd844  0300a0e3  mov r0, #0x3
108bd848  33b8feeb  bl 0x1086b91c
108bd84c  040050e1  cmp r0, r4
108bd850  1300001a  bne 0x108bd8a4
108bd854  0410a0e1  cpy r1, r4
108bd858  0500a0e1  cpy r0, r5
108bd85c  0f5bfeeb  bl 0x108544a0
108bd860  0050a0e1  cpy r5, r0
108bd864  0310a0e3  mov r1, #0x3
108bd868  2d3afeeb  bl 0x1084c124
108bd86c  0040a0e1  cpy r4, r0
108bd870  0d10a0e3  mov r1, #0xd
108bd874  0500a0e1  cpy r0, r5
108bd878  e8dafeeb  bl 0x10874420
108bd87c  040050e1  cmp r0, r4
108bd880  0700008a  bhi 0x108bd8a4
108bd884  1600a0e3  mov r0, #0x16
108bd888  23b8feeb  bl 0x1086b91c
108bd88c  0a0050e3  cmp r0, #0xa
108bd890  0300001a  bne 0x108bd8a4
108bd894  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd898  061ba0e3  mov r1, #0x1800
108bd89c  3600a0e3  mov r0, #0x36
108bd8a0  773dfeea  b 0x1084ce84
108bd8a4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
