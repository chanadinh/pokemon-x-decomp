/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0013aa84(int param_1,int param_2)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  uint uVar10;
  
  uVar5 = _static_::anon_segment_0_5306c();
  iVar6 = FUN_0016b348(*(undefined4 *)(param_1 + 0xc));
  if (iVar6 == 0) {
    uVar8 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc));
    uVar1 = _static_::anon_segment_0_68ae4(param_1);
    uVar2 = FUN_0016b318(*(undefined4 *)(param_1 + 0xc));
    uVar3 = FUN_0016b2ec(*(undefined4 *)(param_1 + 0xc));
    uVar9 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
    _static_::anon_segment_0_67e2c(uVar8,uVar9);
    uVar4 = _static_::anon_segment_0_67e18(0);
    uVar7 = FUN_00168b84(uVar8,uVar1,uVar4,uVar2,uVar3);
  }
  else {
    uVar7 = FUN_00152ea8();
  }
  uVar8 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc));
  uVar1 = _static_::anon_segment_0_68ae4(param_1);
  uVar2 = FUN_0016b318(*(undefined4 *)(param_1 + 0xc));
  uVar3 = FUN_0016b2ec(*(undefined4 *)(param_1 + 0xc));
  uVar9 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
  _static_::anon_segment_0_67e2c(uVar8,uVar9);
  uVar4 = _static_::anon_segment_0_67e18(0);
  uVar8 = FUN_00168b84(uVar8,uVar1,uVar4,uVar2,uVar3);
  FUN_0014fd54(*(undefined4 *)(param_1 + 0xc),uVar8);
  if ((param_2 != 0) && (uVar7 == 0)) {
    return;
  }
  uVar10 = _static_::anon_segment_0_5306c(param_1);
  if (uVar10 < uVar5) {
    if (uVar10 < uVar7) {
      uVar7 = uVar10;
    }
  }
  else {
    uVar7 = uVar7 + (uVar10 - uVar5);
  }
  FUN_0014f930(*(undefined4 *)(param_1 + 0xc),uVar7 & 0xffff);
  return;
}

