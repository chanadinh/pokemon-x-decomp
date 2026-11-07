108bfcf8  38402de9  stmdb sp!,{r3,r4,r5,lr}
108bfcfc  0140a0e1  cpy r4, r1
108bfd00  0250a0e1  cpy r5, r2
108bfd04  0500a0e3  mov r0, #0x5
108bfd08  3c84feeb  bl 0x10860e00
108bfd0c  1e10a0e3  mov r1, #0x1e
108bfd10  0030a0e1  cpy r3, r0
108bfd14  00108de5  str r1, [sp,#0x0]
108bfd18  0520a0e3  mov r2, #0x5
108bfd1c  0510a0e1  cpy r1, r5
108bfd20  0400a0e1  cpy r0, r4
108bfd24  092cfeeb  bl 0x1084ad50
108bfd28  3880bde8  ldmia sp!,{r3,r4,r5,pc}
