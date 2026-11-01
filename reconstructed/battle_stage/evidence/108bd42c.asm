108bd42c  10402de9  stmdb sp!,{r4,lr}
108bd430  0240a0e1  cpy r4, r2
108bd434  0200a0e3  mov r0, #0x2
108bd438  37b9feeb  bl 0x1086b91c
108bd43c  040050e1  cmp r0, r4
108bd440  0500001a  bne 0x108bd45c
108bd444  2100a0e3  mov r0, #0x21
108bd448  33b9feeb  bl 0x1086b91c
108bd44c  8010a0e1  mov r1, r0, lsl #0x1
108bd450  1040bde8  ldmia sp!,{r4,lr}
108bd454  2100a0e3  mov r0, #0x21
108bd458  1da1feea  b 0x108658d4
108bd45c  1080bde8  ldmia sp!,{r4,pc}
