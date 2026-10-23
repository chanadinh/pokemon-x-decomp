0012b284  10402de9  stmdb sp!,{r4,lr}
0012b288  3c409fe5  ldr r4,[0x12b2cc]
0012b28c  000050e3  cmp r0,#0x0
0012b290  cc1094a5  ldrge r1,[r4,#0xcc]
0012b294  000051a1  cmpge r1,r0
0012b298  050000da  ble 0x0012b2b4
0012b29c  c81094e5  ldr r1,[r4,#0xc8]
0012b2a0  800181e0  add r0,r1,r0, lsl #0x3
0012b2a4  041090e5  ldr r1,[r0,#0x4]
0012b2a8  010011e3  tst r1,#0x1
0012b2ac  00009015  ldrne r0,[r0,#0x0]
0012b2b0  0400001a  bne 0x0012b2c8
0012b2b4  6c1094e5  ldr r1,[r4,#0x6c]
0012b2b8  000051e3  cmp r1,#0x0
0012b2bc  0c009f15  ldrne r0,[0x12b2d0]
0012b2c0  31ff2f11  blxne r1
0012b2c4  faffffea  b 0x0012b2b4
0012b2c8  1080bde8  ldmia sp!,{r4,pc}
