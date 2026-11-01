108bfc9c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bfca0  0150a0e1  cpy r5, r1
108bfca4  0240a0e1  cpy r4, r2
108bfca8  0300a0e3  mov r0, #0x3
108bfcac  1aaffeeb  bl 0x1086b91c
108bfcb0  040050e1  cmp r0, r4
108bfcb4  0e00001a  bne 0x108bfcf4
108bfcb8  0410a0e1  cpy r1, r4
108bfcbc  0500a0e1  cpy r0, r5
108bfcc0  f651feeb  bl 0x108544a0
108bfcc4  0510a0e3  mov r1, #0x5
108bfcc8  6e5bfeeb  bl 0x10856a88
108bfccc  000050e3  cmp r0, #0x0
108bfcd0  0700000a  beq 0x108bfcf4
108bfcd4  1b00a0e3  mov r0, #0x1b
108bfcd8  0faffeeb  bl 0x1086b91c
108bfcdc  010050e3  cmp r0, #0x1
108bfce0  0300001a  bne 0x108bfcf4
108bfce4  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bfce8  061ba0e3  mov r1, #0x1800
108bfcec  3200a0e3  mov r0, #0x32
108bfcf0  6334feea  b 0x1084ce84
108bfcf4  7080bde8  ldmia sp!,{r4,r5,r6,pc}
