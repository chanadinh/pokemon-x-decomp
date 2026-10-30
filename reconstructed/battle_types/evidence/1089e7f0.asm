1089e7f0  10402de9  stmdb sp!,{r4,lr}
1089e7f4  0e0051e3  cmp r1, #0xe
1089e7f8  0040a0e1  cpy r4, r0
1089e7fc  01f19f37  ldrcc pc, [pc,r1,lsl #0x2]
1089e800  270000ea  b 0x1089e8a4
1089e83c  0000a0e3  mov r0, #0x0
1089e840  1080bde8  ldmia sp!,{r4,pc}
1089e844  2003a0e1  mov r0, r0, lsr #0x6
1089e848  1080bde8  ldmia sp!,{r4,pc}
1089e84c  a002a0e1  mov r0, r0, lsr #0x5
1089e850  1080bde8  ldmia sp!,{r4,pc}
1089e854  2002a0e1  mov r0, r0, lsr #0x4
1089e858  1080bde8  ldmia sp!,{r4,pc}
1089e85c  a001a0e1  mov r0, r0, lsr #0x3
1089e860  1080bde8  ldmia sp!,{r4,pc}
1089e864  2001a0e1  mov r0, r0, lsr #0x2
1089e868  1080bde8  ldmia sp!,{r4,pc}
1089e86c  a000a0e1  mov r0, r0, lsr #0x1
1089e870  1080bde8  ldmia sp!,{r4,pc}
1089e874  8000a0e1  mov r0, r0, lsl #0x1
1089e878  1080bde8  ldmia sp!,{r4,pc}
1089e87c  0001a0e1  mov r0, r0, lsl #0x2
1089e880  1080bde8  ldmia sp!,{r4,pc}
1089e884  8001a0e1  mov r0, r0, lsl #0x3
1089e888  1080bde8  ldmia sp!,{r4,pc}
1089e88c  0002a0e1  mov r0, r0, lsl #0x4
1089e890  1080bde8  ldmia sp!,{r4,pc}
1089e894  8002a0e1  mov r0, r0, lsl #0x5
1089e898  1080bde8  ldmia sp!,{r4,pc}
1089e89c  0003a0e1  mov r0, r0, lsl #0x6
1089e8a0  1080bde8  ldmia sp!,{r4,pc}
1089e8a4  0020a0e3  mov r2, #0x0
1089e8a8  0210a0e1  cpy r1, r2
1089e8ac  0200a0e1  cpy r0, r2
1089e8b0  b786fdeb  bl 0x10800394
1089e8b4  0400a0e1  cpy r0, r4
1089e8b8  1080bde8  ldmia sp!,{r4,pc}
