/* Unreviewed Ghidra pseudocode; types require assembly review. */

bool _static_::anon_segment_0_2f9a4(int param_1)

{
  bool bVar1;
  
  bVar1 = *(char *)(*(int *)(param_1 + 0xc) + 0xd) == '\0';
  if (bVar1) {
    FUN_0013a768(*(undefined4 *)(param_1 + 0xc));
  }
  return bVar1;
}

