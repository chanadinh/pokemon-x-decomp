108bd678  10402de9  stmdb sp!,{r4,lr}
108bd67c  0240a0e1  cpy r4, r2
108bd680  0300a0e3  mov r0, #0x3
108bd684  a4b8feeb  bl 0x1086b91c
108bd688  040050e1  cmp r0, r4
108bd68c  0700001a  bne 0x108bd6b0
108bd690  2600a0e3  mov r0, #0x26
108bd694  a0b8feeb  bl 0x1086b91c
108bd698  ff0010e3  tst r0, #0xff
108bd69c  0300001a  bne 0x108bd6b0
108bd6a0  1040bde8  ldmia sp!,{r4,lr}
108bd6a4  0a10a0e3  mov r1, #0xa
108bd6a8  2700a0e3  mov r0, #0x27
108bd6ac  88a0feea  b 0x108658d4
108bd6b0  1080bde8  ldmia sp!,{r4,pc}
