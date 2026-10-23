/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168384(undefined4 param_1,uint param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_0016b3d4(param_1,1);
  *(uint *)(iVar1 + 0x34) = *(uint *)(iVar1 + 0x34) & 0xfe0fffff | (param_2 & 0x1f) << 0x14;
  FUN_001685f0(param_1);
  return;
}

