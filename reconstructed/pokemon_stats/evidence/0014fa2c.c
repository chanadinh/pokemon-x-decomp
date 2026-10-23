/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_0014fa2c(int param_1)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  undefined4 uVar5;
  undefined4 uVar6;
  int iVar7;
  int iVar8;
  
  uVar1 = _static_::anon_segment_0_68ae4();
  uVar2 = FUN_00152cec(*(undefined4 *)(param_1 + 0xc));
  uVar3 = FUN_00152c68(*(undefined4 *)(param_1 + 0xc));
  FUN_00152c3c(*(undefined4 *)(param_1 + 0xc));
  uVar5 = FUN_00152fe0(*(undefined4 *)(param_1 + 0xc));
  uVar6 = FUN_00152fa8(*(undefined4 *)(param_1 + 0xc));
  _static_::anon_segment_0_67e2c(uVar5,uVar6);
  uVar4 = _static_::anon_segment_0_67e18(3);
  uVar2 = FUN_0014ff74(uVar1,uVar4,uVar2,uVar3);
  iVar7 = *(int *)(param_1 + 0xc);
  if (*(int *)(iVar7 + 4) != 0) {
    FUN_00168660(iVar7);
    iVar8 = DAT_0014fafc;
    if ((*(ushort *)(*(int *)(iVar7 + 8) + 4) & 4) == 0) {
      iVar8 = *(int *)(iVar7 + 4);
    }
    *(undefined2 *)(iVar8 + 0x10) = uVar2;
    FUN_001685f0(iVar7);
    return;
  }
  return;
}

