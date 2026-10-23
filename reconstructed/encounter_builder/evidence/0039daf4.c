/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 *
_static_::anon_segment_0_29daf4(undefined4 *param_1,undefined4 param_2,undefined4 param_3)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = DAT_0039db5c;
  param_1[1] = 0;
  *param_1 = uVar1;
  param_1[2] = 0;
  param_1[3] = 0;
  uVar1 = FUN_0039a8a4(param_2,param_3);
  param_1[1] = uVar1;
  iVar2 = anon_segment_0_22920(0x10,param_2);
  uVar1 = 0;
  if (iVar2 != 0) {
    uVar1 = FUN_0014ff4c();
  }
  param_1[3] = uVar1;
  FUN_0013a7a0(uVar1,param_1[1],0);
  FUN_0013a8d8(param_1);
  return param_1;
}

