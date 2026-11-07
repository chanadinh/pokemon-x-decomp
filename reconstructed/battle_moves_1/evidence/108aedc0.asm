108aedc0  f0412de9  stmdb sp!,{r4,r5,r6,r7,r8,lr}
108aedc4  0150a0e1  cpy r5, r1
108aedc8  0240a0e1  cpy r4, r2
108aedcc  0300a0e3  mov r0, #0x3
108aedd0  d1f2feeb  bl 0x1086b91c
108aedd4  040050e1  cmp r0, r4
108aedd8  1900001a  bne 0x108aee44
108aeddc  0400a0e3  mov r0, #0x4
108aede0  cdf2feeb  bl 0x1086b91c
108aede4  ff1000e2  and r1, r0, #0xff
108aede8  0500a0e1  cpy r0, r5
108aedec  ab95feeb  bl 0x108544a0
108aedf0  50709fe5  ldr r7, [0x108aee48]
108aedf4  0050a0e3  mov r5, #0x0
108aedf8  0060a0e1  cpy r6, r0
108aedfc  0540a0e1  cpy r4, r5
108aee00  0600a0e1  cpy r0, r6
108aee04  0410d7e7  ldrb r1, [r7,r4]
108aee08  8415ffeb  bl 0x10874420
108aee0c  060050e3  cmp r0, #0x6
108aee10  050080c0  addgt r0, r0, r5
108aee14  065040c2  subgt r5, r0, #0x6
108aee18  014084e2  add r4, r4, #0x1
108aee1c  070054e3  cmp r4, #0x7
108aee20  f6ffff3a  bcc 0x108aee00
108aee24  051185e0  add r1, r5, r5, lsl #0x2
108aee28  3c00a0e3  mov r0, #0x3c
108aee2c  011180e0  add r1, r0, r1, lsl #0x2
108aee30  f041bde8  ldmia sp!,{r4,r5,r6,r7,r8,lr}
108aee34  c80051e3  cmp r1, #0xc8
108aee38  c810a083  movhi r1, #0xc8
108aee3c  3100a0e3  mov r0, #0x31
108aee40  a3dafeea  b 0x108658d4
108aee44  f081bde8  ldmia sp!,{r4,r5,r6,r7,r8,pc}
