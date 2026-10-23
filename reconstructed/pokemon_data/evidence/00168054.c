/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168054(undefined4 param_1,byte param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_001761e8(param_1,1);
  *(byte *)(iVar1 + 0x2d) = param_2 & 0x7f | *(byte *)(iVar1 + 0x2d) & 0x80;
  FUN_001685f0(param_1);
  return;
}

