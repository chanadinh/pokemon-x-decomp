/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined1 FUN_001688c8(undefined4 param_1,undefined4 param_2,uint param_3)

{
  undefined1 uVar1;
  uint uVar2;
  undefined4 uVar3;
  
  uVar2 = param_3;
  if (2 < param_3) {
    uVar2 = 0;
  }
  if (param_3 != 0) {
    if (param_3 == 1) {
      uVar3 = 0x1b;
      goto LAB_00168924;
    }
    if (param_3 == 2) {
      uVar3 = 0x1c;
      goto LAB_00168924;
    }
    uVar2 = 0;
  }
  uVar3 = 0x1a;
LAB_00168924:
  _static_::anon_segment_0_67e2c(param_1,param_2,uVar2);
  uVar1 = _static_::anon_segment_0_67e18(uVar3);
  return uVar1;
}

