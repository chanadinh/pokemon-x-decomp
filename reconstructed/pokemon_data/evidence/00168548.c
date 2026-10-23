/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_00168548(int param_1,undefined4 param_2,undefined4 param_3)

{
  *(undefined4 *)(param_1 + 8) = param_2;
  *(undefined1 *)(param_1 + 0xc) = 0;
  *(undefined4 *)(param_1 + 4) = param_3;
  *(undefined1 *)(param_1 + 0xd) = 0;
  FUN_001685f0();
  return;
}

