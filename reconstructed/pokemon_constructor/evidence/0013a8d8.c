/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0013a8d8(int param_1)

{
  int *piVar1;
  byte bVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  uint uVar7;
  int iVar8;
  uint uVar9;
  undefined4 uVar10;
  undefined4 extraout_r1;
  uint uVar11;
  undefined1 auStack_38 [32];
  
  bVar2 = 0;
  do {
    if (bVar2 < 4) {
      FUN_00168824(*(undefined4 *)(param_1 + 0xc),bVar2,0);
      FUN_001684f4(*(undefined4 *)(param_1 + 0xc),bVar2,0);
      FUN_0014f9d8(*(undefined4 *)(param_1 + 0xc),bVar2,0);
      uVar10 = extraout_r1;
    }
    else {
      uVar10 = 0;
    }
    bVar2 = bVar2 + 1;
  } while (bVar2 < 4);
  uVar10 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc),uVar10);
  uVar3 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
  uVar4 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc));
  uVar5 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
  uVar6 = FUN_0016b45c(*(undefined4 *)(param_1 + 0xc));
  uVar7 = FUN_00168b3c(uVar4,uVar5,uVar6);
  _static_::anon_segment_0_4f2b8(uVar10,uVar3);
  uVar11 = 0;
  iVar8 = _static_::anon_segment_0_4f294();
  if (iVar8 != 0) {
    do {
      uVar9 = _static_::anon_segment_0_4f2a4(uVar11 & 0xffff);
      if (uVar7 < uVar9) break;
      uVar10 = _static_::anon_segment_0_4f310(uVar11 & 0xffff);
      _static_::anon_segment_0_50064(param_1,uVar10);
      uVar11 = uVar11 + 1;
      uVar9 = _static_::anon_segment_0_4f294();
    } while (uVar11 < uVar9);
  }
  uVar10 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc));
  FUN_0014f224(auStack_38,uVar10);
  FUN_001685c0(*(undefined4 *)(param_1 + 0xc),auStack_38);
  FUN_0014f8cc(*(undefined4 *)(param_1 + 0xc),0);
  _static_::anon_segment_0_3a7bc(param_1);
  piVar1 = DAT_0013aa80;
  FUN_001682a4(*(undefined4 *)(param_1 + 0xc),*(undefined1 *)(*DAT_0013aa80 + 0x69));
  FUN_00168594(*(undefined4 *)(param_1 + 0xc),*(undefined1 *)(*piVar1 + 0x6a));
  FUN_001687cc(*(undefined4 *)(param_1 + 0xc),*(undefined1 *)(*piVar1 + 0x68));
  FUN_00168404(*(undefined4 *)(param_1 + 0xc),*(undefined1 *)(*piVar1 + 0x6b));
  return;
}

