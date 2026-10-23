/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168468(undefined4 param_1,char param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_0016b358(param_1,1);
  *(byte *)(iVar1 + 0xd) = *(byte *)(iVar1 + 0xd) & 0xfd | param_2 << 1;
  FUN_001685f0(param_1);
  return;
}

