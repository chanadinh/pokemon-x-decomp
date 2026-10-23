/* Unreviewed Ghidra pseudocode; types require assembly review. */

uint FUN_00168b3c(undefined4 param_1,undefined4 param_2,uint param_3)

{
  uint uVar1;
  uint uVar2;
  
  _static_::anon_segment_0_67db0();
  uVar2 = 1;
  do {
    uVar1 = _static_::anon_segment_0_67f98(uVar2);
    if (param_3 < uVar1) break;
    uVar2 = uVar2 + 1 & 0xff;
  } while (uVar2 < 0x65);
  if ((int)(uVar2 - 1) < 1) {
    uVar2 = 1;
  }
  else {
    uVar2 = uVar2 - 1 & 0xff;
  }
  return uVar2;
}

