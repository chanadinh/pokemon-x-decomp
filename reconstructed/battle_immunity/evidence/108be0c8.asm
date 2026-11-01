108be0c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108be0cc  0160a0e1  cpy r6, r1
108be0d0  0250a0e1  cpy r5, r2
108be0d4  0400a0e3  mov r0, #0x4
108be0d8  0fb6feeb  bl 0x1086b91c
108be0dc  050050e1  cmp r0, r5
108be0e0  2700001a  bne 0x108be184
108be0e4  0300a0e3  mov r0, #0x3
108be0e8  0bb6feeb  bl 0x1086b91c
108be0ec  ff4000e2  and r4, r0, #0xff
108be0f0  0410a0e1  cpy r1, r4
108be0f4  0500a0e1  cpy r0, r5
108be0f8  2a39feeb  bl 0x1084c5a8
108be0fc  000050e3  cmp r0, #0x0
108be100  1f00000a  beq 0x108be184
108be104  040055e1  cmp r5, r4
108be108  1d00000a  beq 0x108be184
108be10c  1200a0e3  mov r0, #0x12
108be110  01b6feeb  bl 0x1086b91c
108be114  7000ffe6  uxth r0, r0
108be118  c508fdeb  bl 0x10800434
108be11c  000050e3  cmp r0, #0x0
108be120  1700000a  beq 0x108be184
108be124  0110a0e3  mov r1, #0x1
108be128  4100a0e3  mov r0, #0x41
108be12c  e89dfeeb  bl 0x108658d4
108be130  000050e3  cmp r0, #0x0
108be134  1200000a  beq 0x108be184
108be138  0520a0e1  cpy r2, r5
108be13c  0310a0e3  mov r1, #0x3
108be140  0600a0e1  cpy r0, r6
108be144  8caffeeb  bl 0x10869f7c
108be148  0040a0e1  cpy r4, r0
108be14c  000090e5  ldr r0, [r0,#0x0]
108be150  30209fe5  ldr r2, [0x108be188]
108be154  0210a0e3  mov r1, #0x2
108be158  020580e3  orr r0, r0, #0x800000
108be15c  000084e5  str r0, [r4,#0x0]
108be160  040084e2  add r0, r4, #0x4
108be164  5a3cfeeb  bl 0x1084d2d4
108be168  0510a0e1  cpy r1, r5
108be16c  040084e2  add r0, r4, #0x4
108be170  493cfeeb  bl 0x1084d29c
108be174  0410a0e1  cpy r1, r4
108be178  0600a0e1  cpy r0, r6
108be17c  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108be180  70a6feea  b 0x10867b48
108be184  7080bde8  ldmia sp!,{r4,r5,r6,pc}
