108bddcc  10402de9  stmdb sp!,{r4,lr}
108bddd0  0240a0e1  cpy r4, r2
108bddd4  0300a0e3  mov r0, #0x3
108bddd8  cfb6feeb  bl 0x1086b91c
108bdddc  040050e1  cmp r0, r4
108bdde0  1700001a  bne 0x108bde44
108bdde4  2a00a0e3  mov r0, #0x2a
108bdde8  cbb6feeb  bl 0x1086b91c
108bddec  010050e3  cmp r0, #0x1
108bddf0  1300001a  bne 0x108bde44
108bddf4  4e00a0e3  mov r0, #0x4e
108bddf8  c7b6feeb  bl 0x1086b91c
108bddfc  000050e3  cmp r0, #0x0
108bde00  0f00001a  bne 0x108bde44
108bde04  1200a0e3  mov r0, #0x12
108bde08  c3b6feeb  bl 0x1086b91c
108bde0c  7040ffe6  uxth r4, r0
108bde10  0400a0e1  cpy r0, r4
108bde14  7e9affeb  bl 0x108a4814
108bde18  000050e3  cmp r0, #0x0
108bde1c  0800001a  bne 0x108bde44
108bde20  0110a0e3  mov r1, #0x1
108bde24  0400a0e1  cpy r0, r4
108bde28  9b09fdeb  bl 0x1080049c
108bde2c  000050e3  cmp r0, #0x0
108bde30  0300001a  bne 0x108bde44
108bde34  1040bde8  ldmia sp!,{r4,lr}
108bde38  0210a0e3  mov r1, #0x2
108bde3c  2b00a0e3  mov r0, #0x2b
108bde40  a39efeea  b 0x108658d4
108bde44  1080bde8  ldmia sp!,{r4,pc}
