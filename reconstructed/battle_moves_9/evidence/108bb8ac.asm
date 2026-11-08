108bb8ac  f0472de9  stmdb sp!,{r4,r5,r6,r7,r8,r9,r10,lr}
108bb8b0  0170a0e1  cpy r7, r1
108bb8b4  0280a0e1  cpy r8, r2
108bb8b8  0090a0e1  cpy r9, r0
108bb8bc  d63ffeeb  bl 0x1084b81c
108bb8c0  080050e3  cmp r0, #0x8
108bb8c4  00f19f37  ldrcc pc, [pc,r0,lsl #0x2]
108bb8c8  0b0000ea  b 0x108bb8fc
108bb8ec  0140a0e3  mov r4, #0x1
108bb8f0  0650a0e3  mov r5, #0x6
108bb8f4  0460a0e1  cpy r6, r4
108bb8f8  120000ea  b 0x108bb948
108bb8fc  0040a0e3  mov r4, #0x0
108bb900  0150a0e3  mov r5, #0x1
108bb904  0460a0e1  cpy r6, r4
108bb908  0e0000ea  b 0x108bb948
108bb90c  0240a0e3  mov r4, #0x2
108bb910  0050a0e3  mov r5, #0x0
108bb914  0660a0e3  mov r6, #0x6
108bb918  0a0000ea  b 0x108bb948
108bb91c  0040a0e3  mov r4, #0x0
108bb920  0250a0e3  mov r5, #0x2
108bb924  0760a0e3  mov r6, #0x7
108bb928  060000ea  b 0x108bb948
108bb92c  0140a0e3  mov r4, #0x1
108bb930  0450a0e1  cpy r5, r4
108bb934  0560a0e3  mov r6, #0x5
108bb938  020000ea  b 0x108bb948
108bb93c  0140a0e3  mov r4, #0x1
108bb940  0550a0e3  mov r5, #0x5
108bb944  0260a0e3  mov r6, #0x2
108bb948  0900a0e1  cpy r0, r9
108bb94c  e03ffeeb  bl 0x1084b8d4
108bb950  010050e3  cmp r0, #0x1
108bb954  00f020e3  nop
108bb958  0a00000a  beq 0x108bb988
108bb95c  020050e3  cmp r0, #0x2
108bb960  0c00000a  beq 0x108bb998
108bb964  030050e3  cmp r0, #0x3
108bb968  1100001a  bne 0x108bb9b4
108bb96c  0d0000ea  b 0x108bb9a8
108bb970  060050e3  cmp r0, #0x6
108bb974  0040a0e3  mov r4, #0x0
108bb978  0350a0e3  mov r5, #0x3
108bb97c  0360a003  moveq r6, #0x3
108bb980  0460a013  movne r6, #0x4
108bb984  efffffea  b 0x108bb948
108bb988  0040a0e3  mov r4, #0x0
108bb98c  0250a0e3  mov r5, #0x2
108bb990  0760a0e3  mov r6, #0x7
108bb994  060000ea  b 0x108bb9b4
108bb998  0140a0e3  mov r4, #0x1
108bb99c  0350a0e3  mov r5, #0x3
108bb9a0  0860a0e3  mov r6, #0x8
108bb9a4  020000ea  b 0x108bb9b4
108bb9a8  0040a0e3  mov r4, #0x0
108bb9ac  0150a0e3  mov r5, #0x1
108bb9b0  0960a0e3  mov r6, #0x9
108bb9b4  000057e3  cmp r7, #0x0
108bb9b8  0040c715  strbne r4, [r7,#0x0]
108bb9bc  000058e3  cmp r8, #0x0
108bb9c0  0600a0e1  cpy r0, r6
108bb9c4  0050c815  strbne r5, [r8,#0x0]
108bb9c8  f087bde8  ldmia sp!,{r4,r5,r6,r7,r8,r9,r10,pc}
