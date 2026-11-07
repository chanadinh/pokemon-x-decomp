108b32c4  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b32c8  0150a0e1  cpy r5, r1
108b32cc  0240a0e1  cpy r4, r2
108b32d0  0300a0e3  mov r0, #0x3
108b32d4  90e1feeb  bl 0x1086b91c
108b32d8  040050e1  cmp r0, r4
108b32dc  0e00001a  bne 0x108b331c
108b32e0  0410a0e1  cpy r1, r4
108b32e4  0500a0e1  cpy r0, r5
108b32e8  6c84feeb  bl 0x108544a0
108b32ec  000090e5  ldr r0, [r0,#0x0]
108b32f0  7f3afdeb  bl 0x10801cf4
108b32f4  ff1060e2  rsb r1, r0, #0xff
108b32f8  20009fe5  ldr r0, [0x108b3320]
108b32fc  011181e0  add r1, r1, r1, lsl #0x2
108b3300  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b3304  8110a0e1  mov r1, r1, lsl #0x1
108b3308  901180e0  umull r1, r0, r0, r1
108b330c  a011b0e1  movs r1, r0, lsr #0x3
108b3310  0110a003  moveq r1, #0x1
108b3314  3100a0e3  mov r0, #0x31
108b3318  6dc9feea  b 0x108658d4
108b331c  7080bde8  ldmia sp!,{r4,r5,r6,pc}
