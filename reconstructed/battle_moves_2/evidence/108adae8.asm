108adae8  70402de9  stmdb sp!,{r4,r5,r6,lr}
108adaec  0060a0e1  cpy r6, r0
108adaf0  0150a0e1  cpy r5, r1
108adaf4  0240a0e1  cpy r4, r2
108adaf8  0300a0e3  mov r0, #0x3
108adafc  86f7feeb  bl 0x1086b91c
108adb00  040050e1  cmp r0, r4
108adb04  1500001a  bne 0x108adb60
108adb08  0410a0e1  cpy r1, r4
108adb0c  0500a0e1  cpy r0, r5
108adb10  629afeeb  bl 0x108544a0
108adb14  0050a0e1  cpy r5, r0
108adb18  0d10a0e3  mov r1, #0xd
108adb1c  3f1affeb  bl 0x10874420
108adb20  0040a0e1  cpy r4, r0
108adb24  0e10a0e3  mov r1, #0xe
108adb28  0500a0e1  cpy r0, r5
108adb2c  3b1affeb  bl 0x10874420
108adb30  0050a0e1  cpy r5, r0
108adb34  0600a0e1  cpy r0, r6
108adb38  b803d0e1  ldrh r0, [r0,#0x38]
108adb3c  bc4efdeb  bl 0x10801634
108adb40  900400e0  mul r0, r0, r4
108adb44  0510a0e1  cpy r1, r5
108adb48  334afdeb  bl 0x1080041c
108adb4c  0010b0e1  movs r1, r0
108adb50  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108adb54  0110a003  moveq r1, #0x1
108adb58  3100a0e3  mov r0, #0x31
108adb5c  5cdffeea  b 0x108658d4
108adb60  7080bde8  ldmia sp!,{r4,r5,r6,pc}
