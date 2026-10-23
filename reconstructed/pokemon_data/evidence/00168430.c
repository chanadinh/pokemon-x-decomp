/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168430(undefined4 param_1,byte param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_0016b358(param_1,1);
  *(byte *)(iVar1 + 0xd) = *(byte *)(iVar1 + 0xd) & 0xfe | param_2;
  FUN_001685f0(param_1);
  return;
}

