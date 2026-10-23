/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint _static_::anon_segment_0_50220(undefined4 param_1,uint param_2)

{
  uint uVar1;
  
  if (3 < param_2) {
    param_2 = 3;
  }
  uVar1 = anon_segment_0_68bbc(param_1,5);
  return (uVar1 & 0xff) +
         (uint)((ulonglong)(uint)((int)(short)(uVar1 & 0xff) * (int)(short)param_2 * 0x14) *
                (ulonglong)DAT_0015025c >> 0x25) & 0xff;
}

