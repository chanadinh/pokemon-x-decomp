1084be58  10402de9  stmdb sp!,{r4,lr}
1084be5c  0040a0e1  cpy r4, r0
1084be60  477f00eb  bl 0x1086bb84
1084be64  0010a0e3  mov r1, #0x0
1084be68  4200a0e3  mov r0, #0x42
1084be6c  e27e00eb  bl 0x1086b9fc
1084be70  8310a0e3  mov r1, #0x83
1084be74  0400a0e1  cpy r0, r4
1084be78  c97e00eb  bl 0x1086b9a4
1084be7c  4200a0e3  mov r0, #0x42
1084be80  a57e00eb  bl 0x1086b91c
1084be84  000050e3  cmp r0, #0x0
1084be88  0200000a  beq 0x1084be98
1084be8c  7b7e00eb  bl 0x1086b880
1084be90  0000a0e3  mov r0, #0x0
1084be94  1080bde8  ldmia sp!,{r4,pc}
1084be98  00f020e3  nop
1084be9c  777e00eb  bl 0x1086b880
1084bea0  1c0094e5  ldr r0, [r4,#0x1c]
1084bea4  0100d0e5  ldrb r0, [r0,#0x1]
1084bea8  1080bde8  ldmia sp!,{r4,pc}
