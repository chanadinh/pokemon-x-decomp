108aecac  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108aecb0  0300a0e3  mov r0, #0x3
108aecb4  0180a0e1  cpy r8, r1
108aecb8  0270a0e1  cpy r7, r2
108aecbc  16f3feeb  bl 0x1086b91c
108aecc0  070050e1  cmp r0, r7
108aecc4  2600001a  bne 0x108aed64
108aecc8  0500a0e3  mov r0, #0x5
108aeccc  12f3feeb  bl 0x1086b91c
108aecd0  006050e2  subs r6, r0, #0x0
108aecd4  0040a0e3  mov r4, #0x0
108aecd8  03a0a083  movhi r10, #0x3
108aecdc  01b0a083  movhi r11, #0x1
108aece0  1f00009a  bls 0x108aed64
108aece4  060084e2  add r0, r4, #0x6
108aece8  ff0000e2  and r0, r0, #0xff
108aecec  0af3feeb  bl 0x1086b91c
108aecf0  ff5000e2  and r5, r0, #0xff
108aecf4  0510a0e1  cpy r1, r5
108aecf8  0800a0e1  cpy r0, r8
108aecfc  e795feeb  bl 0x108544a0
108aed00  0090a0e1  cpy r9, r0
108aed04  00f020e3  nop
108aed08  cdc2feeb  bl 0x1085f844
108aed0c  000050e3  cmp r0, #0x0
108aed10  00f020e3  nop
108aed14  0f00001a  bne 0x108aed58
108aed18  0310a0e3  mov r1, #0x3
108aed1c  0900a0e1  cpy r0, r9
108aed20  589ffeeb  bl 0x10856a88
108aed24  000050e3  cmp r0, #0x0
108aed28  00f020e3  nop
108aed2c  0900000a  beq 0x108aed58
108aed30  0720a0e1  cpy r2, r7
108aed34  0a10a0e3  mov r1, #0xa
108aed38  0800a0e1  cpy r0, r8
108aed3c  8eecfeeb  bl 0x10869f7c
108aed40  04a0c0e5  strb r10, [r0,#0x4]
108aed44  0550c0e5  strb r5, [r0,#0x5]
108aed48  0010a0e1  cpy r1, r0
108aed4c  11b0c0e5  strb r11, [r0,#0x11]
108aed50  0800a0e1  cpy r0, r8
108aed54  7be3feeb  bl 0x10867b48
108aed58  014084e2  add r4, r4, #0x1
108aed5c  060054e1  cmp r4, r6
108aed60  dfffff3a  bcc 0x108aece4
108aed64  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
