/* Unreviewed Ghidra pseudocode; types require assembly review. */

void FUN_001cca9c(undefined1 *param_1,int param_2,int param_3,undefined4 param_4,undefined1 param_5,
                 int param_6,int *param_7)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  undefined4 uVar4;
  int iVar5;
  undefined4 uVar6;
  int *piVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  
  if (param_3 == 0) {
    _static_::anon_segment_0_1e864(0,0);
  }
  piVar7 = *(int **)(param_2 + 0x1c);
  _static_::anon_segment_0_cca50(param_1);
  FUN_0034ee18(param_1,param_2,param_4);
  iVar5 = *(int *)(param_2 + 0x1c);
  iVar1 = _static_::anon_segment_0_22920(0x1c,param_7);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = _static_::anon_segment_0_1294c(iVar1,param_7);
  }
  *(undefined4 *)(param_1 + 0x4c) = uVar2;
  _static_::anon_segment_0_29e7e0(uVar2,*(undefined4 *)(iVar5 + 0x14));
  puVar3 = (undefined4 *)(DAT_001ccbcc + param_6 * 0x18);
  uVar2 = *puVar3;
  uVar4 = puVar3[1];
  uVar9 = puVar3[2];
  uVar6 = puVar3[3];
  uVar8 = puVar3[4];
  *(undefined4 *)(param_1 + 0x40) = puVar3[5];
  *(undefined4 *)(param_1 + 0x2c) = uVar2;
  *(undefined4 *)(param_1 + 0x30) = uVar4;
  *(undefined4 *)(param_1 + 0x34) = uVar9;
  *(undefined4 *)(param_1 + 0x38) = uVar6;
  *(undefined4 *)(param_1 + 0x3c) = uVar8;
  iVar1 = _static_::anon_segment_0_22920(0x1c,param_7);
  uVar2 = 0;
  if (iVar1 != 0) {
    uVar2 = _static_::anon_segment_0_1294c(iVar1,param_7);
  }
  *(undefined4 *)(param_1 + 0x50) = uVar2;
  _static_::anon_segment_0_29e7e0(uVar2,param_3);
  param_1[1] = param_5;
  *param_1 = 0;
  iVar1 = _static_::anon_segment_0_3a1f74(*(undefined4 *)(param_1 + 0x4c));
  if (iVar1 != 0) {
    iVar1 = *piVar7;
    uVar2 = (**(code **)(*param_7 + 0x34))(param_7);
    iVar1 = _static_::anon_segment_0_32d2b4(iVar1 + 0x1ef38,uVar2);
    if (iVar1 == 0) {
      *(uint *)(param_1 + 0xa8) = *(uint *)(param_1 + 0xa8) | 0x40;
    }
  }
  return;
}

