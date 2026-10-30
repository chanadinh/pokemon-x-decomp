00127bd8  4c1090e5  ldr r1, [r0,#0x4c]
00127bdc  c11fa0e1  mov r1, r1, asr #0x1f
00127be0  011091e2  adds r1, r1, #0x1
00127be4  b403d011  ldrhne r0, [r0,#0x34]
00127be8  1eff2f11  bxne lr
00127bec  70402de9  stmdb sp!,{r4,r5,r6,lr}
00127bf0  08d04de2  sub sp, sp, #0x8
00127bf4  041090e5  ldr r1, [r0,#0x4]
00127bf8  0050a0e1  cpy r5, r0
00127bfc  4c0100eb  bl 0x00128134
00127c00  0040a0e1  cpy r4, r0
00127c04  000090e5  ldr r0, [r0,#0x0]
00127c08  241090e5  ldr r1, [r0,#0x24]
00127c0c  0400a0e1  cpy r0, r4
00127c10  31ff2fe1  blx r1
00127c14  000054e3  cmp r4, #0x0
00127c18  0060a0e1  cpy r6, r0
00127c1c  0300000a  beq 0x00127c30
00127c20  000094e5  ldr r0, [r4,#0x0]
00127c24  041090e5  ldr r1, [r0,#0x4]
00127c28  0400a0e1  cpy r0, r4
00127c2c  31ff2fe1  blx r1
00127c30  0000a0e3  mov r0, #0x0
00127c34  b400cde1  strh r0, [sp,#0x4]
00127c38  00008de5  str r0, [sp,#0x0]
00127c3c  04308de2  add r3, sp, #0x4
00127c40  0220a0e3  mov r2, #0x2
00127c44  0610a0e1  cpy r1, r6
00127c48  0500a0e1  cpy r0, r5
00127c4c  020000eb  bl 0x00127c5c
00127c50  b400dde1  ldrh r0, [sp,#0x4]
00127c54  08d08de2  add sp, sp, #0x8
00127c58  7080bde8  ldmia sp!,{r4,r5,r6,pc}
