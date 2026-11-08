108bbc64  70402de9  stmdb sp!,{r4,r5,r6,lr}
108bbc68  0260a0e1  cpy r6, r2
108bbc6c  b803d0e1  ldrh r0, [r0,#0x38]
108bbc70  0050a0e1  cpy r5, r0
108bbc74  0610a0e1  cpy r1, r6
108bbc78  0000a0e3  mov r0, #0x0
108bbc7c  cf45feeb  bl 0x1084d3c0
108bbc80  0040b0e1  movs r4, r0
108bbc84  0f00000a  beq 0x108bbcc8
108bbc88  00f020e3  nop
108bbc8c  b803d0e1  ldrh r0, [r0,#0x38]
108bbc90  050050e1  cmp r0, r5
108bbc94  0400a0e1  cpy r0, r4
108bbc98  0600001a  bne 0x108bbcb8
108bbc9c  4a45feeb  bl 0x1084d1cc
108bbca0  0610a0e1  cpy r1, r6
108bbca4  0000a0e3  mov r0, #0x0
108bbca8  c445feeb  bl 0x1084d3c0
108bbcac  00f020e3  nop
108bbcb0  00f020e3  nop
108bbcb4  010000ea  b 0x108bbcc0
108bbcb8  00f020e3  nop
108bbcbc  ab45feeb  bl 0x1084d370
108bbcc0  0040b0e1  movs r4, r0
108bbcc4  efffff1a  bne 0x108bbc88
108bbcc8  7080bde8  ldmia sp!,{r4,r5,r6,pc}
