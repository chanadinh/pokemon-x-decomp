/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168404(undefined4 param_1,undefined1 param_2)

{
  int iVar1;
  
  FUN_00168660();
  iVar1 = FUN_001761e8(param_1,0);
  *(undefined1 *)(iVar1 + 0x32) = param_2;
  FUN_001685f0(param_1);
  return;
}

