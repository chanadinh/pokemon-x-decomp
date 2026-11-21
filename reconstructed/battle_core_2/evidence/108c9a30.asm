108c9a30  040051e3  cmp r1, #0x4
108c9a34  0800002a  bcs 0x108c9a5c
108c9a38  013281e0  add r3, r1, r1, lsl #0x4
108c9a3c  811283e0  add r1, r3, r1, lsl #0x5
108c9a40  0130d0e7  ldrb r3, [r0,r1]
108c9a44  030052e1  cmp r2, r3
108c9a48  0300002a  bcs 0x108c9a5c
108c9a4c  010080e0  add r0, r0, r1
108c9a50  020080e0  add r0, r0, r2
108c9a54  1900d0e5  ldrb r0, [r0,#0x19]
108c9a58  1eff2fe1  bx lr
108c9a5c  1f00a0e3  mov r0, #0x1f
108c9a60  1eff2fe1  bx lr
