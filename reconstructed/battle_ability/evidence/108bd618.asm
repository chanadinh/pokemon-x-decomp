108bd618  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bd61c  0150a0e1  cpy r5, r1
108bd620  0240a0e1  cpy r4, r2
108bd624  0300a0e3  mov r0, #0x3
108bd628  bbb8feeb  bl 0x1086b91c
108bd62c  040050e1  cmp r0, r4
108bd630  0f00001a  bne 0x108bd674
108bd634  0410a0e1  cpy r1, r4
108bd638  0500a0e1  cpy r0, r5
108bd63c  975bfeeb  bl 0x108544a0
108bd640  0050a0e1  cpy r5, r0
108bd644  0d10a0e3  mov r1, #0xd
108bd648  74dbfeeb  bl 0x10874420
108bd64c  0040a0e1  cpy r4, r0
108bd650  0210a0e3  mov r1, #0x2
108bd654  0500a0e1  cpy r0, r5
108bd658  b13afeeb  bl 0x1084c124
108bd65c  000054e1  cmp r4, r0
108bd660  0300008a  bhi 0x108bd674
108bd664  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bd668  021ba0e3  mov r1, #0x800
108bd66c  3600a0e3  mov r0, #0x36
108bd670  033efeea  b 0x1084ce84
108bd674  7080bde8  ldmia sp!,{r4,r5,r6,pc}
