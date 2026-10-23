/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint _static_::anon_segment_0_3a1658(int param_1,uint param_2)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = FUN_004a0c24(*(undefined4 *)(param_1 + 0xc));
  uVar2 = FUN_0049ffe4(*(undefined4 *)(param_1 + 0xc));
  if (param_2 == 0) {
    uVar2 = uVar1 & ~uVar2;
  }
  if (1 < param_2) {
    if (param_2 == 2) {
      uVar2 = uVar2 | uVar1;
    }
    else {
      uVar2 = 0;
    }
  }
  return uVar2;
}

