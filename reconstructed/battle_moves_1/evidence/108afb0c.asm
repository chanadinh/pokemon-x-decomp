108afb0c  70402de9  stmdb sp!,{r4,r5,r6,lr}
108afb10  08d04de2  sub sp, sp, #0x8
108afb14  0150a0e1  cpy r5, r1
108afb18  0240a0e1  cpy r4, r2
108afb1c  0300a0e3  mov r0, #0x3
108afb20  7deffeeb  bl 0x1086b91c
108afb24  040050e1  cmp r0, r4
108afb28  1900001a  bne 0x108afb94
108afb2c  0410a0e1  cpy r1, r4
108afb30  0500a0e1  cpy r0, r5
108afb34  5992feeb  bl 0x108544a0
108afb38  0060a0e1  cpy r6, r0
108afb3c  0400a0e3  mov r0, #0x4
108afb40  75effeeb  bl 0x1086b91c
108afb44  ff5000e2  and r5, r0, #0xff
108afb48  0020a0e3  mov r2, #0x0
108afb4c  0140a0e3  mov r4, #0x1
108afb50  0d30a0e1  cpy r3, sp
108afb54  0210a0e1  cpy r1, r2
108afb58  0600a0e1  cpy r0, r6
108afb5c  d86ffeeb  bl 0x1084bac4
108afb60  000050e3  cmp r0, #0x0
108afb64  0a00000a  beq 0x108afb94
108afb68  0500dde5  ldrb r0, [sp,#0x5]
108afb6c  050050e1  cmp r0, r5
108afb70  0900001a  bne 0x108afb9c
108afb74  b200dde1  ldrh r0, [sp,#0x2]
108afb78  000050e3  cmp r0, #0x0
108afb7c  0600000a  beq 0x108afb9c
108afb80  3100a0e3  mov r0, #0x31
108afb84  64effeeb  bl 0x1086b91c
108afb88  8010a0e1  mov r1, r0, lsl #0x1
108afb8c  3100a0e3  mov r0, #0x31
108afb90  4fd7feeb  bl 0x108658d4
108afb94  08d08de2  add sp, sp, #0x8
108afb98  7080bde8  ldmia sp!,{r4,r5,r6,pc}
108afb9c  010084e2  add r0, r4, #0x1
108afba0  0420a0e1  cpy r2, r4
108afba4  ff4000e2  and r4, r0, #0xff
108afba8  0d30a0e1  cpy r3, sp
108afbac  0010a0e3  mov r1, #0x0
108afbb0  0600a0e1  cpy r0, r6
108afbb4  c26ffeeb  bl 0x1084bac4
108afbb8  000050e3  cmp r0, #0x0
108afbbc  00f020e3  nop
108afbc0  e8ffff1a  bne 0x108afb68
108afbc4  08d08de2  add sp, sp, #0x8
108afbc8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
