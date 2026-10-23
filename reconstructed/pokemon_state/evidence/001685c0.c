/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_001685c0(int param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  char cVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  undefined4 unaff_r4;
  undefined4 in_lr;
  bool bVar4;
  
  FUN_00168660();
  uVar3 = FUN_0016b3d4(param_1,1);
  _static_::anon_segment_0_679f4(uVar3,param_2);
  cVar1 = *(char *)(param_1 + 0xc);
  bVar4 = cVar1 == '\0';
  if (bVar4) {
    cVar1 = *(char *)(param_1 + 0xd);
  }
  if (bVar4 && cVar1 == '\0') {
    uVar2 = FUN_00174e0c(*(int *)(param_1 + 8) + 8,0xe0);
    *(undefined2 *)(*(int *)(param_1 + 8) + 6) = uVar2;
    FUN_00167fb4(*(undefined4 **)(param_1 + 8) + 2,0xe0,**(undefined4 **)(param_1 + 8),param_4,
                 unaff_r4,in_lr);
    if (*(int *)(param_1 + 4) != 0) {
      FUN_00167fb4(*(int *)(param_1 + 4),0x1c,**(undefined4 **)(param_1 + 8),param_4,unaff_r4,in_lr)
      ;
    }
    *(undefined1 *)(param_1 + 0xc) = 1;
  }
  return;
}

