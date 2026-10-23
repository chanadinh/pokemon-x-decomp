/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint _static_::anon_segment_0_332f00(undefined4 param_1,undefined4 param_2)

{
  undefined1 auStack_20 [20];
  uint local_c;
  
  anon_segment_0_332a24(param_1,param_2,auStack_20);
  if ((local_c & 1) != 0) {
    return (local_c & 0x20) >> 5;
  }
  return 0;
}

