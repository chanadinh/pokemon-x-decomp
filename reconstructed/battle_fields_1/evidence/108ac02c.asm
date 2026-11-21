108ac02c  10402de9  stmdb sp!,{r4,lr}
108ac030  0240a0e1  cpy r4, r2
108ac034  0300a0e3  mov r0, #0x3
108ac038  37fefeeb  bl 0x1086b91c
108ac03c  ff0000e2  and r0, r0, #0xff
108ac040  de7ffeeb  bl 0x1084bfc0
108ac044  040050e1  cmp r0, r4
108ac048  0600001a  bne 0x108ac068
108ac04c  2700a0e3  mov r0, #0x27
108ac050  31fefeeb  bl 0x1086b91c
108ac054  10109fe5  ldr r1, [0x108ac06c]
108ac058  1040bde8  ldmia sp!,{r4,lr}
108ac05c  801001e0  and r1, r1, r0, lsl #0x1
108ac060  2700a0e3  mov r0, #0x27
108ac064  1ae6feea  b 0x108658d4
108ac068  1080bde8  ldmia sp!,{r4,pc}
