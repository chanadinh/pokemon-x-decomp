108b2cfc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108b2d00  0150a0e1  cpy r5, r1
108b2d04  0240a0e1  cpy r4, r2
108b2d08  0300a0e3  mov r0, #0x3
108b2d0c  02e3feeb  bl 0x1086b91c
108b2d10  040050e1  cmp r0, r4
108b2d14  1200001a  bne 0x108b2d64
108b2d18  0410a0e1  cpy r1, r4
108b2d1c  0500a0e1  cpy r0, r5
108b2d20  de85feeb  bl 0x108544a0
108b2d24  0f10a0e3  mov r1, #0xf
108b2d28  bc05ffeb  bl 0x10874420
108b2d2c  ff4000e2  and r4, r0, #0xff
108b2d30  6500a0e3  mov r0, #0x65
108b2d34  a066feeb  bl 0x1084c7bc
108b2d38  320080e2  add r0, r0, #0x32
108b2d3c  24109fe5  ldr r1, [0x108b2d68]
108b2d40  7000ffe6  uxth r0, r0
108b2d44  940000e0  mul r0, r4, r0
108b2d48  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108b2d4c  911080e0  umull r1, r0, r1, r0
108b2d50  8005a0e1  mov r0, r0, lsl #0xb
108b2d54  2018b0e1  movs r1, r0, lsr #0x10
108b2d58  0110a003  moveq r1, #0x1
108b2d5c  3800a0e3  mov r0, #0x38
108b2d60  dbcafeea  b 0x108658d4
108b2d64  7080bde8  ldmia sp!,{r4,r5,r6,pc}
