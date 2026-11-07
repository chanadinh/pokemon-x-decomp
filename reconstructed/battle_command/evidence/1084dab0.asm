1084dab0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
1084dab4  0340a0e1  cpy r4, r3
1084dab8  0070a0e1  cpy r7, r0
1084dabc  040082e0  add r0, r2, r4
1084dac0  080050e3  cmp r0, #0x8
1084dac4  0180a0e1  cpy r8, r1
1084dac8  0250a0e1  cpy r5, r2
1084dacc  030000da  ble 0x1084dae0
1084dad0  0020a0e3  mov r2, #0x0
1084dad4  0210a0e1  cpy r1, r2
1084dad8  0200a0e1  cpy r0, r2
1084dadc  2ccafeeb  bl 0x10800394
1084dae0  000055e3  cmp r5, #0x0
1084dae4  0300001a  bne 0x1084daf8
1084dae8  0020a0e3  mov r2, #0x0
1084daec  0210a0e1  cpy r1, r2
1084daf0  0200a0e1  cpy r0, r2
1084daf4  26cafeeb  bl 0x10800394
1084daf8  000054e3  cmp r4, #0x0
1084dafc  0300001a  bne 0x1084db10
1084db00  0020a0e3  mov r2, #0x0
1084db04  0210a0e1  cpy r1, r2
1084db08  0200a0e1  cpy r0, r2
1084db0c  20cafeeb  bl 0x10800394
1084db10  0100a0e3  mov r0, #0x1
1084db14  1015a0e1  mov r1, r0, lsl r5
1084db18  1004a0e1  mov r0, r0, lsl r4
1084db1c  011041e2  sub r1, r1, #0x1
1084db20  010040e2  sub r0, r0, #0x1
1084db24  ff5001e2  and r5, r1, #0xff
1084db28  050057e1  cmp r7, r5
1084db2c  ff6000e2  and r6, r0, #0xff
1084db30  030000da  ble 0x1084db44
1084db34  0020a0e3  mov r2, #0x0
1084db38  0210a0e1  cpy r1, r2
1084db3c  0200a0e1  cpy r0, r2
1084db40  13cafeeb  bl 0x10800394
1084db44  060058e1  cmp r8, r6
1084db48  030000da  ble 0x1084db5c
1084db4c  0020a0e3  mov r2, #0x0
1084db50  0210a0e1  cpy r1, r2
1084db54  0200a0e1  cpy r0, r2
1084db58  0dcafeeb  bl 0x10800394
1084db5c  050007e0  and r0, r7, r5
1084db60  061008e0  and r1, r8, r6
1084db64  100481e1  orr r0, r1, r0, lsl r4
1084db68  ff0000e2  and r0, r0, #0xff
1084db6c  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
