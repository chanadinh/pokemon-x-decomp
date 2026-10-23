/* Unreviewed Ghidra pseudocode; types require assembly review. */

ulonglong _static_::anon_segment_0_332a7c(int param_1,undefined4 param_2)

{
  uint uVar1;
  byte unaff_r5;
  
  uVar1 = 0;
  while( true ) {
    if ((uVar1 & 0xff) < 100) {
      unaff_r5 = *(byte *)((uVar1 & 0xff) * 0x15 + param_1 + 0x18);
    }
    else {
      param_2 = 0;
    }
    if ((unaff_r5 & 2) != 0) break;
    uVar1 = uVar1 + 1;
    if (99 < (int)uVar1) {
      return 0;
    }
  }
  return CONCAT44(param_2,uVar1) & 0xffffffff000000ff;
}

