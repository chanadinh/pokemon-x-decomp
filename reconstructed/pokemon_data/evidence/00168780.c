/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168780(undefined4 param_1,char param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_0016b358(param_1,1);
  *(byte *)(iVar1 + 0x15) = *(byte *)(iVar1 + 0x15) & 7 | param_2 << 3;
  FUN_001685f0(param_1);
  return;
}

