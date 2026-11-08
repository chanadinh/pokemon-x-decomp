108bf9fc  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bfa00  0150a0e1  cpy r5, r1
108bfa04  0240a0e1  cpy r4, r2
108bfa08  0200a0e3  mov r0, #0x2
108bfa0c  c2affeeb  bl 0x1086b91c
108bfa10  040050e1  cmp r0, r4
108bfa14  2100001a  bne 0x108bfaa0
108bfa18  0500a0e1  cpy r0, r5
108bfa1c  0d31feeb  bl 0x1084be58
108bfa20  020050e3  cmp r0, #0x2
108bfa24  1d00001a  bne 0x108bfaa0
108bfa28  0410a0e1  cpy r1, r4
108bfa2c  0500a0e1  cpy r0, r5
108bfa30  9a52feeb  bl 0x108544a0
108bfa34  9cb7feeb  bl 0x1086d8ac
108bfa38  000050e3  cmp r0, #0x0
108bfa3c  1700000a  beq 0x108bfaa0
108bfa40  0420a0e1  cpy r2, r4
108bfa44  0110a0e3  mov r1, #0x1
108bfa48  0500a0e1  cpy r0, r5
108bfa4c  bd33feeb  bl 0x1084c948
108bfa50  0420a0e1  cpy r2, r4
108bfa54  0a10a0e3  mov r1, #0xa
108bfa58  0500a0e1  cpy r0, r5
108bfa5c  46a9feeb  bl 0x10869f7c
108bfa60  002090e5  ldr r2, [r0,#0x0]
108bfa64  0010a0e1  cpy r1, r0
108bfa68  2630a0e3  mov r3, #0x26
108bfa6c  0430c1e5  strb r3, [r1,#0x4]
108bfa70  0100a0e3  mov r0, #0x1
108bfa74  0540c1e5  strb r4, [r1,#0x5]
108bfa78  1100c1e5  strb r0, [r1,#0x11]
108bfa7c  020482e3  orr r0, r2, #0x2000000
108bfa80  000081e5  str r0, [r1,#0x0]
108bfa84  0500a0e1  cpy r0, r5
108bfa88  2ea0feeb  bl 0x10867b48
108bfa8c  0420a0e1  cpy r2, r4
108bfa90  0500a0e1  cpy r0, r5
108bfa94  7040bde8  ldmia sp!,{r4,r5,r6,lr}
108bfa98  0210a0e3  mov r1, #0x2
108bfa9c  a933feea  b 0x1084c948
108bfaa0  7080bde8  ldmia sp!,{r4,r5,r6,pc}
