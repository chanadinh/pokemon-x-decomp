/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_29e7e0(int param_1,int param_2)

{
  uint uVar1;
  
  uVar1 = 0;
  do {
    anon_segment_0_29a52c(*(undefined4 *)(param_1 + uVar1 * 4),*(undefined4 *)(param_2 + uVar1 * 4))
    ;
    uVar1 = uVar1 + 1;
  } while (uVar1 < 6);
  *(undefined1 *)(param_1 + 0x18) = *(undefined1 *)(param_2 + 0x18);
  return;
}

