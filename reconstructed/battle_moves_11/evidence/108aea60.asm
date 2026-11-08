108aea60  f05f2de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108aea64  0200a0e3  mov r0, #0x2
108aea68  0370a0e1  cpy r7, r3
108aea6c  01a0a0e1  cpy r10, r1
108aea70  0290a0e1  cpy r9, r2
108aea74  a8f3feeb  bl 0x1086b91c
108aea78  090050e1  cmp r0, r9
108aea7c  3a00001a  bne 0x108aeb6c
108aea80  0910a0e1  cpy r1, r9
108aea84  0a00a0e1  cpy r0, r10
108aea88  8496feeb  bl 0x108544a0
108aea8c  00b0a0e1  cpy r11, r0
108aea90  0210a0e3  mov r1, #0x2
108aea94  fb9ffeeb  bl 0x10856a88
108aea98  000050e3  cmp r0, #0x0
108aea9c  3200000a  beq 0x108aeb6c
108aeaa0  0b00a0e1  cpy r0, r11
108aeaa4  5201d0e5  ldrb r0, [r0,#0x152]
108aeaa8  0040a0e3  mov r4, #0x0
108aeaac  005050e2  subs r5, r0, #0x0
108aeab0  0460a0e1  cpy r6, r4
108aeab4  2800009a  bls 0x108aeb5c
108aeab8  0410a0e1  cpy r1, r4
108aeabc  0b00a0e1  cpy r0, r11
108aeac0  689cfeeb  bl 0x10855c68
108aeac4  0080a0e1  cpy r8, r0
108aeac8  00f020e3  nop
108aeacc  00d7ffeb  bl 0x108a46d4
108aead0  000050e3  cmp r0, #0x0
108aead4  00f020e3  nop
108aead8  0800001a  bne 0x108aeb00
108aeadc  0110a0e3  mov r1, #0x1
108aeae0  0800a0e1  cpy r0, r8
108aeae4  6c46fdeb  bl 0x1080049c
108aeae8  000050e3  cmp r0, #0x0
108aeaec  00f020e3  nop
108aeaf0  0200001a  bne 0x108aeb00
108aeaf4  010086e2  add r0, r6, #0x1
108aeaf8  068187e7  str r8, [r7,r6,lsl #0x2]
108aeafc  ff6000e2  and r6, r0, #0xff
108aeb00  010084e2  add r0, r4, #0x1
108aeb04  ff4000e2  and r4, r0, #0xff
108aeb08  040055e1  cmp r5, r4
108aeb0c  e9ffff8a  bhi 0x108aeab8
108aeb10  000056e3  cmp r6, #0x0
108aeb14  1000000a  beq 0x108aeb5c
108aeb18  0600a0e1  cpy r0, r6
108aeb1c  2677feeb  bl 0x1084c7bc
108aeb20  ff0000e2  and r0, r0, #0xff
108aeb24  0910a0e1  cpy r1, r9
108aeb28  000197e7  ldr r0, [r7,r0,lsl #0x2]
108aeb2c  7040ffe6  uxth r4, r0
108aeb30  0420a0e1  cpy r2, r4
108aeb34  0a00a0e1  cpy r0, r10
108aeb38  1b0cffeb  bl 0x10871bac
108aeb3c  0050a0e1  cpy r5, r0
108aeb40  0410a0e1  cpy r1, r4
108aeb44  1200a0e3  mov r0, #0x12
108aeb48  61dbfeeb  bl 0x108658d4
108aeb4c  0510a0e1  cpy r1, r5
108aeb50  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108aeb54  0d00a0e3  mov r0, #0xd
108aeb58  5ddbfeea  b 0x108658d4
108aeb5c  f05fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,lr}
108aeb60  0110a0e3  mov r1, #0x1
108aeb64  4200a0e3  mov r0, #0x42
108aeb68  59dbfeea  b 0x108658d4
108aeb6c  f09fbde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,r11,r12,pc}
