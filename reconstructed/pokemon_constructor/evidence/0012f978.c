/* Unreviewed Ghidra pseudocode; types require assembly review. */

bool _static_::anon_segment_0_2f978(int param_1,uint param_2)

{
  bool bVar1;
  
  bVar1 = ((int)*(char *)(*(int *)(param_1 + 0xc) + 0xd) & param_2) != 0;
  if (bVar1) {
    FUN_0013a720(*(undefined4 *)(param_1 + 0xc));
  }
  return bVar1;
}

