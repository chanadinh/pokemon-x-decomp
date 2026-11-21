108ab6f8  10402de9  stmdb sp!,{r4,lr}
108ab6fc  0240a0e1  cpy r4, r2
108ab700  0200a0e3  mov r0, #0x2
108ab704  8400ffeb  bl 0x1086b91c
108ab708  040050e1  cmp r0, r4
108ab70c  0700001a  bne 0x108ab730
108ab710  3300a0e3  mov r0, #0x33
108ab714  8000ffeb  bl 0x1086b91c
108ab718  000050e3  cmp r0, #0x0
108ab71c  030000da  ble 0x108ab730
108ab720  1040bde8  ldmia sp!,{r4,lr}
108ab724  0110a0e3  mov r1, #0x1
108ab728  4200a0e3  mov r0, #0x42
108ab72c  68e8feea  b 0x108658d4
108ab730  1080bde8  ldmia sp!,{r4,pc}
