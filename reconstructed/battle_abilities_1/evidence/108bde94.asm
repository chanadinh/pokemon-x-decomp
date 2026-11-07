108bde94  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108bde98  0160a0e1  cpy r6, r1
108bde9c  0240a0e1  cpy r4, r2
108bdea0  0400a0e3  mov r0, #0x4
108bdea4  9cb6feeb  bl 0x1086b91c
108bdea8  040050e1  cmp r0, r4
108bdeac  2f00001a  bne 0x108bdf70
108bdeb0  0410a0e1  cpy r1, r4
108bdeb4  0600a0e1  cpy r0, r6
108bdeb8  7859feeb  bl 0x108544a0
108bdebc  6086feeb  bl 0x1085f844
108bdec0  000050e3  cmp r0, #0x0
108bdec4  2900000a  beq 0x108bdf70
108bdec8  1200a0e3  mov r0, #0x12
108bdecc  92b6feeb  bl 0x1086b91c
108bded0  7000ffe6  uxth r0, r0
108bded4  0010a0e3  mov r1, #0x0
108bded8  6f09fdeb  bl 0x1080049c
108bdedc  000050e3  cmp r0, #0x0
108bdee0  2200000a  beq 0x108bdf70
108bdee4  0300a0e3  mov r0, #0x3
108bdee8  8bb6feeb  bl 0x1086b91c
108bdeec  ff5000e2  and r5, r0, #0xff
108bdef0  0510a0e1  cpy r1, r5
108bdef4  0600a0e1  cpy r0, r6
108bdef8  6859feeb  bl 0x108544a0
108bdefc  0070a0e1  cpy r7, r0
108bdf00  0420a0e1  cpy r2, r4
108bdf04  0610a0e3  mov r1, #0x6
108bdf08  0600a0e1  cpy r0, r6
108bdf0c  1ab0feeb  bl 0x10869f7c
108bdf10  0040a0e1  cpy r4, r0
108bdf14  000090e5  ldr r0, [r0,#0x0]
108bdf18  0650c4e5  strb r5, [r4,#0x6]
108bdf1c  0410a0e3  mov r1, #0x4
108bdf20  020580e3  orr r0, r0, #0x800000
108bdf24  000084e5  str r0, [r4,#0x0]
108bdf28  0700a0e1  cpy r0, r7
108bdf2c  f03cfeeb  bl 0x1084d2f4
108bdf30  0010a0e1  cpy r1, r0
108bdf34  0700d4e5  ldrb r0, [r4,#0x7]
108bdf38  b410c4e1  strh r1, [r4,#0x4]
108bdf3c  30209fe5  ldr r2, [0x108bdf74]
108bdf40  010080e3  orr r0, r0, #0x1
108bdf44  0700c4e5  strb r0, [r4,#0x7]
108bdf48  0210a0e3  mov r1, #0x2
108bdf4c  0c0084e2  add r0, r4, #0xc
108bdf50  df3cfeeb  bl 0x1084d2d4
108bdf54  0510a0e1  cpy r1, r5
108bdf58  0c0084e2  add r0, r4, #0xc
108bdf5c  ce3cfeeb  bl 0x1084d29c
108bdf60  0410a0e1  cpy r1, r4
108bdf64  0600a0e1  cpy r0, r6
108bdf68  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108bdf6c  f5a6feea  b 0x10867b48
108bdf70  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
