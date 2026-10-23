/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168028(undefined4 param_1,undefined4 param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_0016b358(param_1,1);
  *(undefined4 *)(iVar1 + 0x10) = param_2;
  FUN_001685f0(param_1);
  return;
}

