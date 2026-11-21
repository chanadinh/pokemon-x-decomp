108adb64  70402de9  stmdb sp!,{r4,r5,r6,lr}
108adb68  0160a0e1  cpy r6, r1
108adb6c  0250a0e1  cpy r5, r2
108adb70  0300a0e3  mov r0, #0x3
108adb74  68f7feeb  bl 0x1086b91c
108adb78  050050e1  cmp r0, r5
108adb7c  1700001a  bne 0x108adbe0
108adb80  0520a0e1  cpy r2, r5
108adb84  1c10a0e3  mov r1, #0x1c
108adb88  0600a0e1  cpy r0, r6
108adb8c  faf0feeb  bl 0x10869f7c
108adb90  0040a0e1  cpy r4, r0
108adb94  0300a0e3  mov r0, #0x3
108adb98  0400c4e5  strb r0, [r4,#0x4]
108adb9c  0500a0e1  cpy r0, r5
108adba0  9e24ffeb  bl 0x10876e20
108adba4  be10d4e1  ldrh r1, [r4,#0xe]
108adba8  080084e5  str r0, [r4,#0x8]
108adbac  30209fe5  ldr r2, [0x108adbe4]
108adbb0  0130a0e3  mov r3, #0x1
108adbb4  2107a0e1  mov r0, r1, lsr #0xe
108adbb8  3030c4e5  strb r3, [r4,#0x30]
108adbbc  0007a0e1  mov r0, r0, lsl #0xe
108adbc0  bc20c4e1  strh r2, [r4,#0xc]
108adbc4  820080e3  orr r0, r0, #0x82
108adbc8  105084e5  str r5, [r4,#0x10]
108adbcc  be00c4e1  strh r0, [r4,#0xe]
108adbd0  0410a0e1  cpy r1, r4
108adbd4  0600a0e1  cpy r0, r6
108adbd8  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108adbdc  d9e7feea  b 0x10867b48
108adbe0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
