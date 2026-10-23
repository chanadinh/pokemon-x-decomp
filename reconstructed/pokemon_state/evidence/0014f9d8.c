/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0014f9d8(undefined4 param_1,uint param_2,undefined1 param_3)

{
  int iVar1;
  
  if (3 < param_2) {
    _static_::anon_segment_0_4f220(0,0);
    return;
  }
  FUN_00168660();
  iVar1 = FUN_0016b3d4(param_1,1);
  *(undefined1 *)(iVar1 + param_2 + 0x22) = param_3;
  FUN_001685f0(param_1);
  return;
}

