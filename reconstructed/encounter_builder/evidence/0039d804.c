/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_29d804(int param_1,undefined2 param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  uVar2 = *(undefined4 *)(param_1 + 0xc);
  FUN_00168660();
  iVar1 = FUN_0016b358(uVar2,1);
  *(undefined2 *)(iVar1 + 2) = param_2;
  FUN_001685f0(uVar2);
  return;
}

