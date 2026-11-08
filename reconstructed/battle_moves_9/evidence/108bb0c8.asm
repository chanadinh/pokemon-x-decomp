108bb0c8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bb0cc  0060a0e1  cpy r6, r0
108bb0d0  08d04de2  sub sp, sp, #0x8
108bb0d4  0150a0e1  cpy r5, r1
108bb0d8  0240a0e1  cpy r4, r2
108bb0dc  0300a0e3  mov r0, #0x3
108bb0e0  0dc2feeb  bl 0x1086b91c
108bb0e4  040050e1  cmp r0, r4
108bb0e8  1000001a  bne 0x108bb130
108bb0ec  0110a0e3  mov r1, #0x1
108bb0f0  0500a0e1  cpy r0, r5
108bb0f4  7f43feeb  bl 0x1084bef8
108bb0f8  0600a0e1  cpy r0, r6
108bb0fc  b803d0e1  ldrh r0, [r0,#0x38]
108bb100  0060a0e1  cpy r6, r0
108bb104  0410a0e1  cpy r1, r4
108bb108  0500a0e1  cpy r0, r5
108bb10c  11e0feeb  bl 0x10873158
108bb110  0010a0e3  mov r1, #0x0
108bb114  0020a0e1  cpy r2, r0
108bb118  00108de5  str r1, [sp,#0x0]
108bb11c  04108de5  str r1, [sp,#0x4]
108bb120  0630a0e1  cpy r3, r6
108bb124  0210a0e1  cpy r1, r2
108bb128  0500a0e1  cpy r0, r5
108bb12c  7887fdeb  bl 0x1081cf14
108bb130  08d08de2  add sp, sp, #0x8
108bb134  7080bde8  ldmia sp!,{r4,r5,r6,pc}
