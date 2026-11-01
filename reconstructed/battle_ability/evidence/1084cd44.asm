1084cd44  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1084cd48  0260a0e1  cpy r6, r2
1084cd4c  0300a0e3  mov r0, #0x3
1084cd50  01a0a0e1  cpy r10, r1
1084cd54  0390a0e1  cpy r9, r3
1084cd58  ef7a00eb  bl 0x1086b91c
1084cd5c  060050e1  cmp r0, r6
1084cd60  3a00001a  bne 0x1084ce50
1084cd64  0950a0e1  cpy r5, r9
1084cd68  3980a0e3  mov r8, #0x39
1084cd6c  0610a0e1  cpy r1, r6
1084cd70  0a00a0e1  cpy r0, r10
1084cd74  f79800eb  bl 0x10873158
1084cd78  0a0050e3  cmp r0, #0xa
1084cd7c  1500000a  beq 0x1084cdd8
1084cd80  071c80e3  orr r1, r0, #0x700
1084cd84  0520a0e1  cpy r2, r5
1084cd88  0a00a0e1  cpy r0, r10
1084cd8c  a3fbffeb  bl 0x1084bc20
1084cd90  007050e2  subs r7, r0, #0x0
1084cd94  0040a0e3  mov r4, #0x0
1084cd98  0e00009a  bls 0x1084cdd8
1084cd9c  0410d5e7  ldrb r1, [r5,r4]
1084cda0  060051e1  cmp r1, r6
1084cda4  0700000a  beq 0x1084cdc8
1084cda8  0a00a0e1  cpy r0, r10
1084cdac  bb1d00eb  bl 0x108544a0
1084cdb0  1110a0e3  mov r1, #0x11
1084cdb4  00f020e3  nop
1084cdb8  989d00eb  bl 0x10874420
1084cdbc  080050e1  cmp r0, r8
1084cdc0  00f020e3  nop
1084cdc4  2200000a  beq 0x1084ce54
1084cdc8  010084e2  add r0, r4, #0x1
1084cdcc  ff4000e2  and r4, r0, #0xff
1084cdd0  070054e1  cmp r4, r7
1084cdd4  f0ffff3a  bcc 0x1084cd9c
1084cdd8  0950a0e1  cpy r5, r9
1084cddc  3a80a0e3  mov r8, #0x3a
1084cde0  0610a0e1  cpy r1, r6
1084cde4  0a00a0e1  cpy r0, r10
1084cde8  da9800eb  bl 0x10873158
1084cdec  0a0050e3  cmp r0, #0xa
1084cdf0  00f020e3  nop
1084cdf4  1500000a  beq 0x1084ce50
1084cdf8  071c80e3  orr r1, r0, #0x700
1084cdfc  0520a0e1  cpy r2, r5
1084ce00  0a00a0e1  cpy r0, r10
1084ce04  85fbffeb  bl 0x1084bc20
1084ce08  007050e2  subs r7, r0, #0x0
1084ce0c  0040a0e3  mov r4, #0x0
1084ce10  0e00009a  bls 0x1084ce50
1084ce14  0410d5e7  ldrb r1, [r5,r4]
1084ce18  060051e1  cmp r1, r6
1084ce1c  0700000a  beq 0x1084ce40
1084ce20  0a00a0e1  cpy r0, r10
1084ce24  9d1d00eb  bl 0x108544a0
1084ce28  1110a0e3  mov r1, #0x11
1084ce2c  00f020e3  nop
1084ce30  7a9d00eb  bl 0x10874420
1084ce34  080050e1  cmp r0, r8
1084ce38  00f020e3  nop
1084ce3c  0400000a  beq 0x1084ce54
1084ce40  010084e2  add r0, r4, #0x1
1084ce44  ff4000e2  and r4, r0, #0xff
1084ce48  070054e1  cmp r4, r7
1084ce4c  f0ffff3a  bcc 0x1084ce14
1084ce50  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
1084ce54  1200a0e3  mov r0, #0x12
1084ce58  af7a00eb  bl 0x1086b91c
1084ce5c  7000ffe6  uxth r0, r0
1084ce60  00f020e3  nop
1084ce64  5ccefeeb  bl 0x108007dc
1084ce68  020050e3  cmp r0, #0x2
1084ce6c  00f020e3  nop
1084ce70  f6ffff1a  bne 0x1084ce50
1084ce74  f047bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
1084ce78  061ba0e3  mov r1, #0x1800
1084ce7c  3600a0e3  mov r0, #0x36
1084ce80  0000a0e1  cpy r0, r0
