1084ad50  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1084ad54  0060a0e1  cpy r6, r0
1084ad58  0150a0e1  cpy r5, r1
1084ad5c  18409de5  ldr r4, [sp,#0x18]
1084ad60  0270a0e1  cpy r7, r2
1084ad64  0380a0e1  cpy r8, r3
1084ad68  0400a0e3  mov r0, #0x4
1084ad6c  ea8200eb  bl 0x1086b91c
1084ad70  050050e1  cmp r0, r5
1084ad74  2600001a  bne 0x1084ae14
1084ad78  4700a0e3  mov r0, #0x47
1084ad7c  e68200eb  bl 0x1086b91c
1084ad80  000050e3  cmp r0, #0x0
1084ad84  2200001a  bne 0x1084ae14
1084ad88  1200a0e3  mov r0, #0x12
1084ad8c  e28200eb  bl 0x1086b91c
1084ad90  7000ffe6  uxth r0, r0
1084ad94  0010a0e3  mov r1, #0x0
1084ad98  bfd5feeb  bl 0x1080049c
1084ad9c  000050e3  cmp r0, #0x0
1084ada0  1b00000a  beq 0x1084ae14
1084ada4  0400a0e1  cpy r0, r4
1084ada8  d20400eb  bl 0x1084c0f8
1084adac  000050e3  cmp r0, #0x0
1084adb0  0400001a  bne 0x1084adc8
1084adb4  0110a0e3  mov r1, #0x1
1084adb8  0600a0e1  cpy r0, r6
1084adbc  ca0400eb  bl 0x1084c0ec
1084adc0  000050e3  cmp r0, #0x0
1084adc4  1200000a  beq 0x1084ae14
1084adc8  0520a0e1  cpy r2, r5
1084adcc  0b10a0e3  mov r1, #0xb
1084add0  0600a0e1  cpy r0, r6
1084add4  687c00eb  bl 0x10869f7c
1084add8  0040a0e1  cpy r4, r0
1084addc  000090e5  ldr r0, [r0,#0x0]
1084ade0  b470c4e1  strh r7, [r4,#0x4]
1084ade4  0010a0e3  mov r1, #0x0
1084ade8  088084e5  str r8, [r4,#0x8]
1084adec  020580e3  orr r0, r0, #0x800000
1084adf0  0c10c4e5  strb r1, [r4,#0xc]
1084adf4  000084e5  str r0, [r4,#0x0]
1084adf8  0300a0e3  mov r0, #0x3
1084adfc  c68200eb  bl 0x1086b91c
1084ae00  0f00c4e5  strb r0, [r4,#0xf]
1084ae04  0410a0e1  cpy r1, r4
1084ae08  0600a0e1  cpy r0, r6
1084ae0c  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
1084ae10  4c7300ea  b 0x10867b48
1084ae14  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
