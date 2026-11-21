108ba7f8  10402de9  stmdb sp!,{r4,lr}
108ba7fc  0240a0e1  cpy r4, r2
108ba800  0300a0e3  mov r0, #0x3
108ba804  44c4feeb  bl 0x1086b91c
108ba808  040050e1  cmp r0, r4
108ba80c  0300001a  bne 0x108ba820
108ba810  1040bde8  ldmia sp!,{r4,lr}
108ba814  0110a0e3  mov r1, #0x1
108ba818  4300a0e3  mov r0, #0x43
108ba81c  2cacfeea  b 0x108658d4
108ba820  1080bde8  ldmia sp!,{r4,pc}
