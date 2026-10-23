/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 DllField::anon_segment_0_e47d8(int param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  if (*(int *)(param_1 + 0x6c) == 0) {
    _static_::anon_segment_0_4f220(0,0);
    return 0;
  }
  iVar1 = 0;
  if (0 < *(int *)(param_1 + 0x6c)) {
    do {
      FUN_15ce4a10(param_1,param_2,param_1 + iVar1 * 0x12,param_3);
      iVar1 = iVar1 + 1;
    } while (iVar1 < *(int *)(param_1 + 0x6c));
  }
  return 1;
}

