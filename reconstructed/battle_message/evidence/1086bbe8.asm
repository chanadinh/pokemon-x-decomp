1086bbe8  04e02de5  str lr, [sp,#-0x4]!
1086bbec  b220d1e1  ldrh r2, [r1,#0x2]
1086bbf0  7c329fe5  ldr r3, [0x1086be74]
1086bbf4  14d04de2  sub sp, sp, #0x14
1086bbf8  010912e3  tst r2, #0x4000
1086bbfc  7fe002e2  and lr, r2, #0x7f
1086bc00  0500000a  beq 0x1086bc1c
1086bc04  20c091e5  ldr r12, [r1,#0x20]
1086bc08  01005ee3  cmp lr, #0x1
1086bc0c  3000000a  beq 0x1086bcd4
1086bc10  02005ee3  cmp lr, #0x2
1086bc14  5f00000a  beq 0x1086bd98
1086bc18  030000ea  b 0x1086bc2c
1086bc1c  01005ee3  cmp lr, #0x1
1086bc20  0400000a  beq 0x1086bc38
1086bc24  02005ee3  cmp lr, #0x2
1086bc28  0b00000a  beq 0x1086bc5c
1086bc2c  14d08de2  add sp, sp, #0x14
1086bc30  0000a0e3  mov r0, #0x0
1086bc34  04f09de4  ldr pc, [sp],#0x4
1086bc38  b0c0d1e1  ldrh r12, [r1,#0x0]
1086bc3c  0229a0e1  mov r2, r2, lsl #0x12
1086bc40  043081e2  add r3, r1, #0x4
1086bc44  a22ca0e1  mov r2, r2, lsr #0x19
1086bc48  0c10a0e1  cpy r1, r12
1086bc4c  daefffeb  bl 0x10867bbc
1086bc50  14d08de2  add sp, sp, #0x14
1086bc54  0100a0e3  mov r0, #0x1
1086bc58  04f09de4  ldr pc, [sp],#0x4
1086bc5c  0229a0e1  mov r2, r2, lsl #0x12
1086bc60  a2cca0e1  mov r12, r2, lsr #0x19
1086bc64  b020d1e1  ldrh r2, [r1,#0x0]
1086bc68  01005ce3  cmp r12, #0x1
1086bc6c  00308d05  streq r3, [sp,#0x0]
1086bc70  1500000a  beq 0x1086bccc
1086bc74  02005ce3  cmp r12, #0x2
1086bc78  0600000a  beq 0x1086bc98
1086bc7c  03005ce3  cmp r12, #0x3
1086bc80  0800000a  beq 0x1086bca8
1086bc84  04005ce3  cmp r12, #0x4
1086bc88  0b00000a  beq 0x1086bcbc
1086bc8c  0c0090e5  ldr r0, [r0,#0xc]
1086bc90  6a10a0e3  mov r1, #0x6a
1086bc94  570000ea  b 0x1086bdf8
1086bc98  08c091e5  ldr r12, [r1,#0x8]
1086bc9c  04308de5  str r3, [sp,#0x4]
1086bca0  00c08de5  str r12, [sp,#0x0]
1086bca4  080000ea  b 0x1086bccc
1086bca8  08c081e2  add r12, r1, #0x8
1086bcac  00509ce8  ldmia r12,{r12,lr}
1086bcb0  08308de5  str r3, [sp,#0x8]
1086bcb4  00508de8  stmia sp,{r12,lr}
1086bcb8  030000ea  b 0x1086bccc
1086bcbc  08c081e2  add r12, r1, #0x8
1086bcc0  030a9cec  vldmia r12, {s0,s1,s2}
1086bcc4  030a8dec  vstmia sp, {s0,s1,s2}
1086bcc8  0c308de5  str r3, [sp,#0xc]
1086bccc  043091e5  ldr r3, [r1,#0x4]
1086bcd0  edffffea  b 0x1086bc8c
1086bcd4  0229a0e1  mov r2, r2, lsl #0x12
1086bcd8  a2eca0e1  mov lr, r2, lsr #0x19
1086bcdc  b020d1e1  ldrh r2, [r1,#0x0]
1086bce0  01005ee3  cmp lr, #0x1
1086bce4  0a00000a  beq 0x1086bd14
1086bce8  02005ee3  cmp lr, #0x2
1086bcec  0e00000a  beq 0x1086bd2c
1086bcf0  03005ee3  cmp lr, #0x3
1086bcf4  1400000a  beq 0x1086bd4c
1086bcf8  04005ee3  cmp lr, #0x4
1086bcfc  1a00000a  beq 0x1086bd6c
1086bd00  00308de5  str r3, [sp,#0x0]
1086bd04  0c0090e5  ldr r0, [r0,#0xc]
1086bd08  0c30a0e1  cpy r3, r12
1086bd0c  6b10a0e3  mov r1, #0x6b
1086bd10  380000ea  b 0x1086bdf8
1086bd14  041091e5  ldr r1, [r1,#0x4]
1086bd18  0a008de8  stmia sp,{r1,r3}
1086bd1c  0c0090e5  ldr r0, [r0,#0xc]
1086bd20  0c30a0e1  cpy r3, r12
1086bd24  6b10a0e3  mov r1, #0x6b
1086bd28  320000ea  b 0x1086bdf8
1086bd2c  041081e2  add r1, r1, #0x4
1086bd30  024091e8  ldmia r1,{r1,lr}
1086bd34  08308de5  str r3, [sp,#0x8]
1086bd38  0c30a0e1  cpy r3, r12
1086bd3c  02408de8  stmia sp,{r1,lr}
1086bd40  0c0090e5  ldr r0, [r0,#0xc]
1086bd44  6b10a0e3  mov r1, #0x6b
1086bd48  2a0000ea  b 0x1086bdf8
1086bd4c  041081e2  add r1, r1, #0x4
1086bd50  030a91ec  vldmia r1, {s0,s1,s2}
1086bd54  6b10a0e3  mov r1, #0x6b
1086bd58  030a8dec  vstmia sp, {s0,s1,s2}
1086bd5c  0c308de5  str r3, [sp,#0xc]
1086bd60  0c0090e5  ldr r0, [r0,#0xc]
1086bd64  0c30a0e1  cpy r3, r12
1086bd68  220000ea  b 0x1086bdf8
1086bd6c  08e081e2  add lr, r1, #0x8
1086bd70  030a9eec  vldmia lr, {s0,s1,s2}
1086bd74  04e08de2  add lr, sp, #0x4
1086bd78  030a8eec  vstmia lr, {s0,s1,s2}
1086bd7c  10308de5  str r3, [sp,#0x10]
1086bd80  041091e5  ldr r1, [r1,#0x4]
1086bd84  0c30a0e1  cpy r3, r12
1086bd88  00108de5  str r1, [sp,#0x0]
1086bd8c  0c0090e5  ldr r0, [r0,#0xc]
1086bd90  6b10a0e3  mov r1, #0x6b
1086bd94  170000ea  b 0x1086bdf8
1086bd98  0229a0e1  mov r2, r2, lsl #0x12
1086bd9c  a2eca0e1  mov lr, r2, lsr #0x19
1086bda0  b020d1e1  ldrh r2, [r1,#0x0]
1086bda4  01005ee3  cmp lr, #0x1
1086bda8  0d00000a  beq 0x1086bde4
1086bdac  02005ee3  cmp lr, #0x2
1086bdb0  1400000a  beq 0x1086be08
1086bdb4  03005ee3  cmp lr, #0x3
1086bdb8  1a00000a  beq 0x1086be28
1086bdbc  04005ee3  cmp lr, #0x4
1086bdc0  2000000a  beq 0x1086be48
1086bdc4  00308de5  str r3, [sp,#0x0]
1086bdc8  0c0090e5  ldr r0, [r0,#0xc]
1086bdcc  0c30a0e1  cpy r3, r12
1086bdd0  6c10a0e3  mov r1, #0x6c
1086bdd4  01feffeb  bl 0x1086b5e0
1086bdd8  00f020e3  nop
1086bddc  00f020e3  nop
1086bde0  050000ea  b 0x1086bdfc
1086bde4  041091e5  ldr r1, [r1,#0x4]
1086bde8  0a008de8  stmia sp,{r1,r3}
1086bdec  0c0090e5  ldr r0, [r0,#0xc]
1086bdf0  0c30a0e1  cpy r3, r12
1086bdf4  6c10a0e3  mov r1, #0x6c
1086bdf8  f8fdffeb  bl 0x1086b5e0
1086bdfc  14d08de2  add sp, sp, #0x14
1086be00  0100a0e3  mov r0, #0x1
1086be04  04f09de4  ldr pc, [sp],#0x4
1086be08  041081e2  add r1, r1, #0x4
1086be0c  020a91ec  vldmia r1, {s0,s1}
1086be10  6c10a0e3  mov r1, #0x6c
1086be14  020a8dec  vstmia sp, {s0,s1}
1086be18  08308de5  str r3, [sp,#0x8]
1086be1c  0c0090e5  ldr r0, [r0,#0xc]
1086be20  0c30a0e1  cpy r3, r12
1086be24  f3ffffea  b 0x1086bdf8
1086be28  041081e2  add r1, r1, #0x4
1086be2c  030a91ec  vldmia r1, {s0,s1,s2}
1086be30  6c10a0e3  mov r1, #0x6c
1086be34  030a8dec  vstmia sp, {s0,s1,s2}
1086be38  0c308de5  str r3, [sp,#0xc]
1086be3c  0c0090e5  ldr r0, [r0,#0xc]
1086be40  0c30a0e1  cpy r3, r12
1086be44  ebffffea  b 0x1086bdf8
1086be48  08e081e2  add lr, r1, #0x8
1086be4c  030a9eec  vldmia lr, {s0,s1,s2}
1086be50  04e08de2  add lr, sp, #0x4
1086be54  030a8eec  vstmia lr, {s0,s1,s2}
1086be58  10308de5  str r3, [sp,#0x10]
1086be5c  041091e5  ldr r1, [r1,#0x4]
1086be60  0c30a0e1  cpy r3, r12
1086be64  00108de5  str r1, [sp,#0x0]
1086be68  0c0090e5  ldr r0, [r0,#0xc]
1086be6c  6c10a0e3  mov r1, #0x6c
1086be70  e0ffffea  b 0x1086bdf8
