/* Unreviewed Ghidra pseudocode; types require assembly review. */

undefined4 FUN_0014f224(undefined4 param_1,uint param_2)

{
  int *piVar1;
  int iVar2;
  undefined4 uVar3;
  
  piVar1 = DAT_0014f290;
  iVar2 = *DAT_0014f290;
  if (param_2 < 0x2d4) {
    uVar3 = FUN_00167c38(param_2,0);
    uVar3 = _static_::anon_segment_0_3a19c(piVar1[1],uVar3,iVar2);
  }
  else {
    uVar3 = 0;
  }
  _static_::anon_segment_0_679f4(param_1,*(undefined4 *)(*piVar1 + 4));
  return uVar3;
}

