1086fd30  012180e0  add r2, r0, r1, lsl #0x2
1086fd34  0030a0e3  mov r3, #0x0
1086fd38  04402de5  str r4, [sp,#-0x4]!
1086fd3c  0c3082e5  str r3, [r2,#0xc]
1086fd40  383082e5  str r3, [r2,#0x38]
1086fd44  643082e5  str r3, [r2,#0x64]
1086fd48  01c181e0  add r12, r1, r1, lsl #0x2
1086fd4c  483282e5  str r3, [r2,#0x248]
1086fd50  743282e5  str r3, [r2,#0x274]
1086fd54  0320a0e1  cpy r2, r3
1086fd58  1f10a0e3  mov r1, #0x1f
1086fd5c  8c0180e0  add r0, r0, r12, lsl #0x3
1086fd60  03c180e0  add r12, r0, r3, lsl #0x2
1086fd64  013083e2  add r3, r3, #0x1
1086fd68  022082e2  add r2, r2, #0x2
1086fd6c  034180e0  add r4, r0, r3, lsl #0x2
1086fd70  90108ce5  str r1, [r12,#0x90]
1086fd74  0a0052e3  cmp r2, #0xa
1086fd78  013083e2  add r3, r3, #0x1
1086fd7c  901084e5  str r1, [r4,#0x90]
1086fd80  f6ffff3a  bcc 0x1086fd60
1086fd84  04409de4  ldr r4, [sp],#0x4
1086fd88  1eff2fe1  bx lr
