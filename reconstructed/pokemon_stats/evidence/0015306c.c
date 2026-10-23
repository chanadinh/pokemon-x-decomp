/* Unreviewed Ghidra pseudocode; types require assembly review. */

void _static_::anon_segment_0_5306c(int param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  int iVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  
  iVar5 = FUN_0016b348(*(undefined4 *)(param_1 + 0xc));
  if (iVar5 != 0) {
    FUN_00152f54();
    return;
  }
  uVar6 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc));
  uVar1 = anon_segment_0_68ae4(param_1);
  uVar2 = FUN_0016b318(*(undefined4 *)(param_1 + 0xc));
  uVar3 = FUN_0016b2ec(*(undefined4 *)(param_1 + 0xc));
  uVar7 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
  anon_segment_0_67e2c(uVar6,uVar7);
  uVar4 = anon_segment_0_67e18(0);
  FUN_00168b84(uVar6,uVar1,uVar4,uVar2,uVar3);
  return;
}

